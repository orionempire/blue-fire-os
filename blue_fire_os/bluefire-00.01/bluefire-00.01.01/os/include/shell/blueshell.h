/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: blueshell.h
 *	Created: Feb 23, 2012
 *	Last Update: Feb 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef BLUESHELL_H_
#define BLUESHELL_H_

#define SHELL_PROMPT	"\r[root@localhost \\]# "
#define CMD_ARG		(cmd+strlen(commands[i].name)+1)

#define SHELL_COMMANDS	( sizeof(commands)/sizeof(CMD) )


#define SH_HELP		0
#define SH_UNAME	1
#define SH_CLEAR	2
#define SH_REBOOT	3
#define SH_DUMP		4
#define SH_READ		5
#define SH_WRITE	6
#define SH_PAGES	7
#define SH_MEM		8
#define SH_CHECKMEM	9
#define SH_FRAMES	10
#define SH_EXEC		11
#define SH_V86EXEC	12
#define SH_TEST		13
#define SH_CPUID	14
#define SH_MOUNT	15
#define SH_LS		16
#define SH_CAT		17
#define SH_CD		18
#define SH_PS		19
#define SH_KILL		20
#define SH_PWD		21


// Public Function declarations

void shell( s32int argc, char **argv);
#endif /* BLUESHELL_H_ */
