;**********************************************************
;	bluefire-os
;	Version: 00.00.05
;	Author: David Davidson
;	Name: interrupt_top.asm
;	Last update: 2012-01-30
;	Purpose: holds the entery point of functions called from itd.
;	Usage:
;***********************************************************
[BITS 32]
%ifndef INTERRUPT_REQUEST_S
%define INTERRUPT_REQUEST_S

%define KERNEL_DATA_SEG 0x10

%macro BUILD_IRQ 1
	push	%1		;put the IRQ number on the stack
	jmp	COMMON_ISR
%endmacro

%endif

GLOBAL irq_00, irq_01, irq_02, irq_03, irq_04, irq_05, irq_06, irq_07
GLOBAL irq_08, irq_09, irq_0A, irq_0B, irq_0C, irq_0D, irq_0E, irq_0F
GLOBAL irq_unhandled

;Declared in os/kernel/interrupt_handler.c
EXTERN interrupt_handler_middle

irq_00:
	BUILD_IRQ 0x00
irq_01:
	BUILD_IRQ 0x01
irq_02:
	BUILD_IRQ 0x02
irq_03:
	BUILD_IRQ 0x03
irq_04:
	BUILD_IRQ 0x04
irq_05:
	BUILD_IRQ 0x05
irq_06:
	BUILD_IRQ 0x06
irq_07:
	BUILD_IRQ 0x07
irq_08:
	BUILD_IRQ 0x08
irq_09:
	BUILD_IRQ 0x09
irq_0A:
	BUILD_IRQ 0x0A
irq_0B:
	BUILD_IRQ 0x0B
irq_0C:
	BUILD_IRQ 0x0C
irq_0D:
	BUILD_IRQ 0x0D
irq_0E:
	BUILD_IRQ 0x0E
irq_0F:
	BUILD_IRQ 0x0F
irq_unhandled:
	BUILD_IRQ 0xFF


; On entry the stack looked like (<top><return>cs, <return>eflags <Bottom>) 	; pushed by cpu
; after the macro above (<top> IRQ, <return>eip, <return>cs, <return>eflags <Bottom> ; IRQ was pushed by macro above
COMMON_ISR:

	push	gs		; save selectors
	push	fs
	push	es
	push	ds
	pushad			; push all general registers

	; The stack now looks like (<top> edi, esi, ebp, <old>esp, ebx, edx, ecx, eax, ds, es fs, gs, IRQ, <return>eip, <return>cs, <return>eflags <Bottom>)

	mov	ebx, KERNEL_DATA_SEG	; Update selectors
	mov	ds, ebx
	mov	es, ebx
	mov	fs, ebx
	mov	gs, ebx		; Update segment registers

	push 	esp		; Needed for (irq_context_t *context)

	; The stack now looks like (<top> <new>esp, edi, esi, ebp, <old>esp, ebx, edx, ecx, eax, ds, es fs, gs, IRQ, <return>eip, <return>cs, <return>eflags <Bottom>)
	; With this stack set up the call is equivlent to the c function...
	; void interrupt_handler_middle( interrupt_context_t *context);		//using include/interrupt_handler.h/interrupt_context_t

	call	interrupt_handler_middle

	pop esp

	popad			; Restore general purpose registers
	pop	ds			; Restore selectors
	pop	es
	pop	fs
	pop	gs

	; the stack must be adjusted exactly to make sure the return Ip is at the top so iretd works
	add	sp, 4	; disgard IRQ number added in macro above.

	iretd
