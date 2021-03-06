;**********************************************************
;	bluefire-os
;	Version: 00.00.05
;	Author: David Davidson
;	Name: execption_top.asm
;	Last update: 2012-01-30
;	Purpose: holds the entery point of functions called from itd.
;	Usage:
;***********************************************************
[BITS 32]
%ifndef EXCEPTION_S
%define EXCEPTION_S

%define KERNEL_DATA_SEG 0x10

%macro BUILD_EXC 1
	push dword %1
	jmp COMMON_EXC
%endmacro

%macro BUILD_NOCODE_EXC 1
	push dword 0
	push dword %1
	jmp COMMON_EXC
%endmacro

%endif

GLOBAL exc_00, exc_01, exc_02, exc_03, exc_04, exc_05, exc_06, exc_07
GLOBAL exc_08, exc_09, exc_0A, exc_0B, exc_0C, exc_0D, exc_0E, exc_0F
GLOBAL exc_unhandled

;Declared in os/kernel/exception_handler.c
EXTERN exception_handler_middle


exc_00:
	BUILD_NOCODE_EXC 0x00	; Division by 0
exc_01:
	BUILD_NOCODE_EXC 0x01	; Debug trap
exc_02:
	BUILD_NOCODE_EXC 0x02	; Non maskable interrupt
exc_03:
	BUILD_NOCODE_EXC 0x03	; Breakpoint instruction
exc_04:
	BUILD_NOCODE_EXC 0x04	; Overflow
exc_05:
	BUILD_NOCODE_EXC 0x05	; Bound range exceeded
exc_06:
	BUILD_NOCODE_EXC 0x06	; Invalid opcode
exc_07:
	BUILD_NOCODE_EXC 0x07	; No math
exc_08:
	BUILD_EXC 0x08			; Double fault
exc_09:
	BUILD_NOCODE_EXC 0x09	; 80x87 overflow
exc_0A:
	BUILD_EXC 0x0A			; Invalid TSS
exc_0B:
	BUILD_EXC 0x0B			; Segment not present
exc_0C:
	BUILD_EXC 0x0C			; Stack fault
exc_0D:
	BUILD_EXC 0x0D			; General protection fault
exc_0E:
	BUILD_EXC 0x0E			; Page fault
exc_0F:
	BUILD_NOCODE_EXC 0x0F	; Unknown exception
exc_unhandled:
	BUILD_NOCODE_EXC 0xFF	; Unhandled exception!

COMMON_EXC:
	push	gs				; save selectors
	push	fs
	push    ds
	push    es
	pushad					; push all general registers (EAX, ECX, EDX, EBX, EBP, ESP, EBP, ESI, and EDI )

	mov     eax, KERNEL_DATA_SEG	; Update selectors
	mov     ds, eax
	mov     es, eax
	mov		fs, eax
	mov		gs, eax

	push 	esp		;needed for (excpeption_context_t *context)


	; must match include/exception_handler.h/exception_context

	; The stack now looks like (<top> <new>esp, edi, esi, ebp, <old>esp, ebx, edx, ecx, eax, ds, es fs, gs, EXC, err_code, <return>eip, <return>cs, <return>eflags <Bottom>)
	; With this stack set up the call is equivlent to the c function...
	; void exception_handler_middle( exception_context_t *context);		//using include/exception_handler.h/exception_context_t

	call	exception_handler_middle

	pop		esp

	popad					; Restore general purpose registers
	pop	es					; Restore selectors
	pop	ds
	pop	fs
	pop	gs

	add	esp, 8				; Skip exception number and error code pushed in macro above.

	iretd
