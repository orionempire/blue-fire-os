#! /bin/bash

# Author		: David Davidson
# Create Date	: 11/23/2011
# Purpose		: Master build script
# Build Platform: Fedora 14 running in Virtual Box 4
# PreReq		: None
# Output		: blue_fire_master.img that can be mounted as a floppy in Bochs (or any vm platform)

echo "Creating blank image....."
dd if=/dev/zero of=build/blue_fire_master.img bs=512 count=2880

echo "Compiling binaries...."
nasm stage1/stage1.asm -o build/stage1.bin -l build/stage1.lst

#write bootloader to first sector (0)
dd if=build/stage1.bin of=build/blue_fire_master.img conv=notrunc
