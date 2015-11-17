/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: common_include.h
 *	Created: Dec 14, 2011
 *	Purpose: Header files that we want usable by every file in kernel
 *  Usage: #include <common_include.h>
***************************************************************************/
#ifndef COMMON_INCLUDE_H_
#define COMMON_INCLUDE_H_

//should go first for use by all other headers.
#include <common.h>

#include <atomic.h>
#include <clock.h>
#include <console.h>
#include <errorno.h>
#include <exception_handler.h>
#include <i386.h>
#include <interrupt_controller.h>
#include <interrupt_handler.h>
#include <kernel.h>
#include <kernel_map.h>
#include <kmalloc.h>
#include <mem.h>
#include <paging.h>
#include <queue.h>
#include <sched.h>
#include <semaphore.h>
#include <shell/blueshell.h>
#include <spinlock.h>
#include <stdarg.h>
#include <task.h>
#include <umalloc.h>
#include <drivers/dma/dma.h>
#include <drivers/floppy/floppy.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/video/video.h>
#include <fs/ext2/ext2.h>
#include <lib/ctype.h>
#include <lib/scanf.h>
#include <lib/string.h>
#include <lib/vsnprintf.h>

#endif /* COMMON_INCLUDE_H_ */
