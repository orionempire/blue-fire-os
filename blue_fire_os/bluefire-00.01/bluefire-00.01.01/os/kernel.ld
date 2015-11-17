/*
	kernel.ld
	- BlueFire OS linker script - 
*/ 
OUTPUT_FORMAT("elf32-i386")

VIRTUAL_KERNEL_START = 0xC0000000;
PHYSICAL_KERNEL_START = 0x100000;
KERNEL_STACK_SIZE = 0x10000;
PAGE_SIZE = 0x1000;

ENTRY (_start)
SECTIONS
{
	.text VIRTUAL_KERNEL_START : AT(PHYSICAL_KERNEL_START)
	{
		KERNEL_TEXT = .;
		*(.text*)
		*(.rodata*)
		. = ALIGN(PAGE_SIZE);
		KERNEL_END_TEXT = .;
	}

	.data : AT(PHYSICAL_KERNEL_START + (KERNEL_DATA - KERNEL_TEXT))
	{
		KERNEL_DATA = .;
		*(.data*)
		. = ALIGN(PAGE_SIZE);
		KERNEL_END_DATA = .;
	}

	.bss : AT(PHYSICAL_KERNEL_START + (KERNEL_BSS - KERNEL_TEXT))
	{
		_stack = .;
		. += KERNEL_STACK_SIZE;
		_stack_end = .;
		KERNEL_END_STACK = .;

		KERNEL_BSS = .;
		*(.bss*)
		*(COMMON*)
		. = ALIGN(PAGE_SIZE);
		KERNEL_END_BSS = .;
	}
	. = ALIGN(PAGE_SIZE);
	KERNEL_END_CODE = .;
}
