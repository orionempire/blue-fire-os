#! /bin/bash

nohup  bochs -f bochsrc.bxrc &

gdb -ix gdbinit