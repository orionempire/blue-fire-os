#!/bin/bash
# Starts bochs using the image created by master_build.sh

IMAGE=blue_fire_master.img
BOCHS_LOG=bochs_log.txt
BOCHS_CONFIG=bochsrc.bxrc
COM1_LOG=com1.out


set -e # fail as soon as one command fails

# check if an old config file exists, if so, remove it
if [ -e $BOCHS_CONFIG ]; then
    rm $BOCHS_CONFIG
fi

# create the config file for bochs
CONFIG="megs:       128
display_library: 	sdl
romimage: 			file="/usr/share/bochs/BIOS-bochs-latest"
vgaromimage: 		file="/usr/share/bochs/VGABIOS-lgpl-latest"
boot: 				floppy
floppy_bootsig_check: disabled=0
floppya: 			type=1_44, 1_44="./$IMAGE", status=inserted, write_protected=0
log:            	$BOCHS_LOG
clock: 				sync=none, time0=local
cpu: 				count=1, ips=4000000, reset_on_triple_fault=1, ignore_bad_msrs=1
com1:           	enabled=0, mode=file, dev=$COM1_LOG"
if [ -z "$1" ]; then 
	echo "$CONFIG" > $BOCHS_CONFIG
	bochs -f $BOCHS_CONFIG -q &
elif [ "$1" -eq "16" ] || [ "$1" -eq "32" ] || [ "$1" -eq "64" ] ; then
	echo "$CONFIG" > $BOCHS_CONFIG
	echo "gdbstub: enabled=1, port=1234, text_base=0, data_base=0, bss_base=0" >> $BOCHS_CONFIG
	bochs -f $BOCHS_CONFIG -q &
	gnome-terminal --command="gdb -ix gdb_bochs_scheme_$1" --geometry=120x60
else 
	echo "usage"
fi

rm $BOCHS_CONFIG

# check if an old log file exists, if so, remove it
if [ -e $BOCHS_LOG ]; then
    rm $BOCHS_LOG
fi

# remove the old log
if [ -e $COM1_LOG ]; then
    rm -f $COM1_LOG
fi

exit 0

