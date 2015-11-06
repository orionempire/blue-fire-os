#####################################################################
# bluefire-os
# Version 00.00.03
# Author: David Davidson
# Last update: 12/20/2011 
# Purpose: To handle the build of our kernel and any sub dependencies.
# Usage : Entry point must be specified first in SOURCES, every directory 
#		used must follow. Special consideration must be takin if any source file 
#		contains the word main in its title
#####################################################################

# we need to build a list of every c and assembly source file used. Add seperate 
# entires for any new directory containing c source files and s source files.

# You would think that the ENTRY (_start) in kernel.ld ensures that start goes first
# but all it really does is mark _start as the entry point. Since this code uses a flat format
# for simplicity start.asm must be the first entry on the build line. 
SOURCES=assembly/start.asm
# Build a list of all primary source files using /bin/sh
SOURCES+=$(shell ls kernel/*.c assembly/*.asm |grep -vi "start.asm")

# add all locations other than primary to list of source files. 
SOURCES+=$(shell ls kernel/lib/*.c kernel/drivers/video/*.c kernel/drivers/dma/*.c)	

# Build a list of every object that is going to be created.
# Iterate throught the list of source files and convert them all to .o files
OBJECTS += $(SOURCES:.c=.o)	
OBJECTS := $(OBJECTS:.asm=.o)
		
INCDIR	=./include
LDSCRIPT=kernel.ld
NASM	=nasm -f elf
GCC		=gcc -m32 -c -Wall -I$(INCDIR) -fno-stack-protector -nostdinc -fno-builtin -nostdlib
LD		=ld -melf_i386 -g -T $(LDSCRIPT) -nostdlib
		
# The final product of our build will be a file called bf_kernel32.elf. Build a debug
# listing from it then extract a simplified usable file called bf_kernel32.bin
all: blue_fire_kernel32.elf clean
	objdump -D -l -x -t ../build/blue_fire_kernel32.elf > ../build/blue_fire_kernel32.lst
	objcopy -I elf32-i386 -O binary ../build/blue_fire_kernel32.elf ../build/blue_fire_kernel32.bin
	
blue_fire_kernel32.elf: $(OBJECTS) 	
	$(LD) -o ../build/$@ $(OBJECTS)

# use nasm on all of the assembly source files
%.o: %.asm
	$(NASM) -o $@ $<		

# use gcc on all of the c language source files	
%.o: %.c
	$(GCC) -o $@ $<

# Every file used by SOURCES must be accounted for here.
clean:
	rm -f kernel/*.o assembly/*.o kernel/lib/*.o kernel/drivers/video/*.o kernel/drivers/dma/*.o
	
# A clean up that wont be called from our build script
scrub: clean
	rm -f ../build/blue_fire*

debug:
	echo $(OBJECTS) 	
	echo $(SOURCES)
	