;**********************************************************
;	bluefire-os
;	Version: 00.00.04
;	Author: David Davidson
;	Name: start.asm
;	Last update: 2012-01-10
;	Purpose:
;	Usage: Must be in protect mode.
;***********************************************************

; Tell assembler to generate 32 bit byte code. Any code in this
; Module must be called after in Protect mode.
[BITS 32]

GLOBAL _start
GLOBAL var_system_memory_amount
GLOBAL stage2_return_address
; defined in  kernel/main.c
[EXTERN k_main]
; defined in kernel.ld
[EXTERN K_STACK_END]


; Kernel virtual start address
%define K_VIR_START		0xC0000000

;Kernel physiscal start address
%define K_PHYS_START	0x100000

; TODO -> document memory map
%define K_PDE			0x1000	; Page directory
%define K_PTE			0x2000	; Kernel page table #0 (4MB)
%define I_PTE			0x3000	; First 4MB Identity-map page table

; Paging constants
%define	P_PRESENT 		0x01
%define P_WRITE			0x02
%define P_USER			0x04
%define PAGE_SIZE		0x1000

; functions can't be called yet so this must be delcared inline.
%macro memset 3
; void memset(void *dest_ptr, int val, dword count);
; o32 a32 forces the opperand and addressing of the string command into 32 bit mode.
	mov edi, %1
	mov eax, %2
	mov ecx, %3
	cld
	o32 a32 rep stosb
%endmacro

; unused but available for future use.
%macro memcpy 3
; void memcpy(void *dest_ptr, void *org_ptr, dword count);
	mov edi, %1
	mov esi, %2
	mov ecx, %3
	cld
	o32 a32 rep movsb
%endmacro


;***********************************************************
; It all starts here
; Note - no variable can be accessed until paging is enabled as
; they are labled (linked) starting from 0xC0000000 but the code is
; intially running as labled 0x1000000
;***********************************************************
_start:
	; Jump over the data section
	jmp entry
	; variables used to preserve information passed by stage2.
	; not_used_space is there to align the next 2 variable.
	; jmp entry 's opcode is 2 bytes, not_used_spacer is another 2.
	not_used_space				DW	0x0000
	var_system_memory_amount	DD	0x00000000
	stage2_return_address		DD 	0x00000000

entry:
	; Turn off floppy motor
	mov	dx, 0x03F2
	mov	al, 0x0C
	out	dx, al

setup_and_enable_paging:
	; Zero out the future page directory, so there are no random entries
	; pointing to phantom memory
	; -> 0x1000 - 0x1FFF : 0x0
	memset K_PDE, 0, PAGE_SIZE

;***********************************************************
; Set up the first 4MB as identity mapped memory.
; memory written here will write to the same physical location
; 0x00000000-0x003fffff -> 0x0000000000000000-0x00000000003fffff
;***********************************************************
	;          Fill      :     With
	; -> 0x3000 - 0x3FFF : 0x0(0 - 3FF)003
	mov	eax, (P_PRESENT | P_WRITE)
	mov	ecx, 1024
	mov	edi, I_PTE
	cld
	; Fill the page table.
Map_I_PTE:
	stosd
	add	eax, PAGE_SIZE
	loop	Map_I_PTE
	; Write the filled page table in the page directory.
	; -> 0x1000 : 0x3007
	mov dword [K_PDE], I_PTE | P_PRESENT | P_WRITE | P_USER

;***********************************************************
; Set up the kernel page table. The 4 MB starting at 1 MB as mapped to
; Logical addresses starting with 0xC to physical addresses 0x + 1MB.
; So writing to 0xC0000001 will put it in physical location 0x00010001
; 0xc0000000-0xc03fffff -> 0x0000000000100000-0x00000000004fffff
;***********************************************************
	;          Fill      :     With
	; -> 0x2000 - 0x2FFF : 0x0(100 - 4FF)003
	mov	eax, (K_PHYS_START | P_PRESENT | P_WRITE)
	mov	ecx, 1024
	mov	edi, K_PTE
	; Fill the page table.
Map_K_PTE:
	stosd
	add	eax, PAGE_SIZE
	loop	Map_K_PTE
	; Write the filled page table in the page directory.
	; -> 0x1C00 : 0x2007
	mov dword [K_PDE+(K_VIR_START/(PAGE_SIZE*1024))*4], K_PTE | P_PRESENT | P_WRITE | P_USER


;
;***********************************************************
; Map page directory into itself. this allows no maintenance while ensuring
; page directory sanity, if it is manipulted as it acts as its own page table.
; 0xfffff000-0xffffffff -> 0x0000000000001000-0x0000000000001fff
; 0xfff00000-0xfff00fff -> 0x0000000000002000-0x0000000000002fff
; 0xffc00000-0xffc00fff -> 0x0000000000003000-0x0000000000003fff
;***********************************************************
	; -> 0x1FFC : 0x1007
	mov dword [K_PDE+1023*4], K_PDE | P_PRESENT | P_WRITE | P_USER

;***********************************************************
; Enable paging
;***********************************************************
	; Setup the Page Directory Base Register
	mov	eax, K_PDE
	mov	cr3, eax
	; enable
	mov	eax, cr0
	or	eax, 0x80010000 ; CR0.PG = CR0.WP = 1
	mov	cr0, eax

; The registers are set on jump so a self jump is executed
	jmp	0x08:paging_enabled

paging_enabled:
	; Update segment registers
	mov	eax, 0x10
	mov	ds, eax
	mov	es, eax
	mov	ss, eax
	nop
	mov	fs, eax
	mov	gs, eax

;***********************************************************
; preserve the variables passed by stage 2,then move the stack
; to the end of the kernel as calculated by kernel.ld.
;***********************************************************
	; unused but preserved.
	pop DWORD [stage2_return_address]
	; Store system memory that was passed on the stack
	pop DWORD [var_system_memory_amount]
	; Initialize kernel stack pointer
	mov	esp, K_STACK_END

	; Jump to the kernel main in kernel/main.c
	jmp	k_main

