;**********************************************************
;	bluefire-os
;	Version: 00.00.03
;	Author: David Davidson
;	Name: load_kernel.asm
;	Last update: 2011-11-20
;	Purpose: To load a kernel from a fixed location on a floppy
;			to a fixed location in upper memory.
;	Usage: Unreal mode must have been enabled. Memory location 0x10000
;			must be availabel for use as a buffer. Kernel must be flat
;			arrayed on floppy sectors 800 to 2800
;***********************************************************


; ------ The physical description of a floppy ------
; The constants that represent the structure of a floppy drive
cst_drive_number			DB	0		; We only support floppy drive A
cst_heads_per_cylinder		DW	2		; A floppy can only have one plater (height of data)
cst_sectors_per_track		DW	18		; A floppy has fixed amt of sectors on a track	(width of data)
cst_bytes_per_sector		DW 	512		; evrey sector has 512 bytes init (density of data)

; ------ Memory locations used as variables ------
;Variables used to return values from function LBACHS
calculated_sector 			DB 0x00
calculated_head   			DB 0x00
calculated_cylinder			DB 0x00

; ------ Memory locations used as counters ------
; Starting location of kernel on disk. Fixed by build script.
var_current_disk_sector			DW 800
; Running counter of where in upper memory the kernel is currently
; being loaded. The segment is increased to keep the counter from
; increasing past a number that can be handled by 16 bits.
var_current_buffer_segment		DW 0
; Running counter of how many groups of 125 disk sectors are left to load.
;var_sector_groups_left_to_load	DW 0x10
var_sector_groups_left_to_load	DW 0x3


load_kernel:
	; Since unreal mode is activated the code can access above 1 MB but
	; the bois interrupts that are needed to load the data off the disk
	; can only be used as if in real mode. So the code Loads
	; 16 groups of 125 disk blocks from the floppy to upper memory
	; using lower memory for each chunk.
	.load_kenel_loop:
		; --------------------------------------------------
		; Load a group of 125 disk sectors to lower memory
		; --------------------------------------------------

		; use 1000:0000(0x10000) as the low memory buffer
		mov		ax, 0x1000
		mov		es, ax
		mov		bx, 0x0000
		; load the 125 disk sectors we are up to
		mov		ax, [var_current_disk_sector]
		mov		cx, 125

		call	read_sectors

		; --------------------------------------------------
		; Copy the buffer to upper memory
		; --------------------------------------------------

		; load the current running upper memory address
		mov		ax, [var_current_buffer_segment]
		mov		es, ax

		; Initalize source segment to be safe
		mov 	ax, 0
		mov		ds, ax

		; The source is 1000:0000 (0x10000), the destination is es:0x100000
		mov		esi, 0x10000
		mov		edi, 0x100000
		; copy the whole (sector group X sector size (125 x 512 =  0xFA00)) buffer
		mov		ecx, 0xFA00
		; we are using unreal memory location in real mode so we use the menomic a32
		; to generate 32 bit code.
		a32 	rep	 movsb

		;inc counters
		add	word [var_current_disk_sector], 125
		add word [var_current_buffer_segment], 0xFA0

		;check iteration
		mov		cx,  [var_sector_groups_left_to_load]
		dec		word [var_sector_groups_left_to_load]
		loop .load_kenel_loop

	ret


;***********************************************************
;	read_sectors:
;	-> AX 		=> Starting Sector
;	-> CX 		=> Amount of sectors to read
;				The buffer is segment limited so
;				must be less then or equal to 128 or buffer
;				will wrap over.
;	-> ES:BX 	=> Buffer to read to
;***********************************************************
read_sectors:
	.read_sectors_start:
		mov		di,0x0005						; Five retries in case of error
	.sector_loop:
		push 	ax								; Save the registers that we are going to use.
		push 	bx
		push 	cx
		call LBACHS								; Calculate what the starting sector is
		mov     ah, 0x02						; 02 Bios read sector function
		mov     al, 0x01 						; Read one sector at a time
		mov     ch, BYTE [calculated_cylinder]	; cylinder as returned by LBACHS
        mov     cl, BYTE [calculated_sector]	; sector as returned by LBACHS
        mov     dh, BYTE [calculated_head]		; head as returned by LBACHS
        mov     dl, BYTE [cst_drive_number]		; drive is always 0
		int     0x13							; invoke BIOS 0x13 AH=0x02
		jnc     .success                        ; test carry flag against a read error
		xor     ax, ax							; 0 is BIOS reset disk
		int     0x13							; invoke BIOS 0x13 AH=0x00
		dec     di								; decrement error counter
		pop     cx
		pop     bx
		pop     ax
		jnz     .sector_loop 					; if less the 5 tries, attempt to read again
		call 	fatal_failure
	.success:
		mov     si, cst_msg_progress
		call    print
		pop     cx
		pop     bx
		pop     ax
		add     bx, WORD [cst_bytes_per_sector] ; queue next buffer
		inc     ax                              ; queue next sector
		loop    .read_sectors_start             ; Decreases CX and jums if not zero.
		ret


;***********************************************************
; 	LBACHS
;	Convert LBA to CHS
; 	AX -> LBA Address to convert
;
;	Returns <-
; 		calculated_sector 	= (logical sector mod sectors per track) + 1
; 		calculated_head   	= (logical sector / sectors per track) MOD number of heads
; 		calculated_cylinder = logical sector / (sectors per track * number of heads)
;
;***********************************************************
LBACHS:
    xor     dx, dx                              ; Zero out dx
    div     WORD [cst_sectors_per_track]		; Calculate sector against DX:AX store quotient->AX remainder->DX
    inc     dl                                  ; Adjust for sector 0
    mov     BYTE [calculated_sector], dl		; Store the result.
    xor     dx, dx                              ; Zero out dx
    div     WORD [cst_heads_per_cylinder]		; calculate sector against DX:AX store quotient->AX remainder->DX
    mov     BYTE [calculated_head], dl			; Store the result.
    mov     BYTE [calculated_cylinder], al		; Store the result.
	ret
