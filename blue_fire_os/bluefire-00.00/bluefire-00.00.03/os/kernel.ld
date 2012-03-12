/*
	kernel.ld
	- BlueFire OS linker script - 
*/ 
OUTPUT_FORMAT("elf32-i386")

K_VIR_START = 0xC0000000;
K_PHYS_START = 0x100000;
K_STACK_SIZE = 0x10000;
PAGE_SIZE = 0x1000;

ENTRY (k_main)

SECTIONS
{
	.text K_VIR_START : AT(K_PHYS_START)
	{
		KERNEL_TEXT = .;
		*(.text*)
		*(.rodata*)
		. = ALIGN(PAGE_SIZE);
		KERNEL_END_TEXT = .;
	}

	.data : AT(K_PHYS_START + (KERNEL_DATA - KERNEL_TEXT))
	{
		KERNEL_DATA = .;
		*(.data*)
		. = ALIGN(PAGE_SIZE);
		KERNEL_END_DATA = .;
	}

	.bss : AT(K_PHYS_START + (KERNEL_BSS - KERNEL_TEXT))
	{
		_stack = .;
		. += K_STACK_SIZE;
		_stack_end = .;
		K_STACK_END = .;

		KERNEL_BSS = .;
		*(.bss*)
		*(COMMON*)
		. = ALIGN(PAGE_SIZE);
		KERNEL_END_BSS = .;
	}
	. = ALIGN(PAGE_SIZE);
	KERNEL_TOP = .;
}
