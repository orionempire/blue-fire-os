;*****************************************************************************
;	bluefire-os
;	Version: 00.00.07
;	Author: David Davidson
;	Name: exit.asm
;	Last update: 2015-10-27
;	Purpose:
;	Usage:
;*****************************************************************************
[BITS 32]

GLOBAL __task_exit_point

section .text  align=4096

; Exit point for task and threads.
; This is the only page of the kernel that must be
; available for execution from the user privilege level.
__task_exit_point:
	push	__task_exit_point
	push	eax		; Push the return value.
syscall:
	xor	eax, eax	; Invoke the exit syscall.
	int	0x80
	jmp	syscall

section align=4096
__task_exit_point_end:
