#! /bin/bash

# Author		: David Davidson
# Create Date	: 11/23/2011
# Purpose		: Master build script
# Build Platform: Fedora 14 running in Virtual Box 4
# PreReq		: None
# Output		: blue_fire_master.img that can be mounted as a floppy in Bochs (or any vm platform)

# --- Build the binaries ---
echo "Compiling binaries...."
nasm stage1/stage1.asm -o stage1/stage1.bin -l build/stage1.lst
nasm stage2/stage2.asm -o stage2/stage2.bin -l build/stage2.lst

# --- Build the disk ---
# create the final image filled with zeroes
echo "Creating blank image....."
rm -f build/blue_fire_master.img
dd if=/dev/zero of=build/blue_fire_master.img bs=512 count=2880

# build the filesystem. Specify 1400 file system blocks (2800 disk sectors) out of 1440 to leave room for stage2. 
losetup /dev/loop0 build/blue_fire_master.img
echo "Making filesystem (reserved last 40 sectors)....."
mkfs -v -t ext2 /dev/loop0 1400

# write bootloader (stage1) to first sector (0)
dd if=stage1/stage1.bin of=build/blue_fire_master.img conv=notrunc
# write bootstrap (stage2) to the end of the disk. There is no check for overflow.
dd if=stage2/stage2.bin of=build/blue_fire_master.img bs=1024 seek=1400 conv=notrunc

# --- Clean up ---
#sleep 3
echo "Cleaning Up....."
rm -f stage1/stage1.bin
rm -f stage2/stage2.bin
losetup -d /dev/loop0


#make -C stage2  > build/stage2.lst
#make -C stage2 clean