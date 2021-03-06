/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: blueshell.c
 *	Created: Feb 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

typedef struct CMD {
	u08int index;
	s08int *name;
	s08int *help;
} CMD;

static CMD commands[22] = {
{ SH_EXEC, "bg", "Execute a 32-bit program in background"},
{ SH_CAT, "cat", "Concatenate FILE to standard output"},
{ SH_CD, "cd", "Change the current directory"},
{ SH_CHECKMEM, "checkmem &", "Create a background task to check frames stack integrity"},
{ SH_CLEAR, "clear", "Clear the screen"},
{ SH_CPUID, "cpuid", "Show informations about the CPU - Only for IA-32 INTEL(R)"},
{ SH_DUMP, "dump", "Register dump"},
{ SH_FRAMES, "frames", "Dump free frames"},
{ SH_HELP, "help", "Show this help"},
{ SH_KILL, "kill", "Kill a task"},
{ SH_LS, "ls", "List informations about FILEs"},
{ SH_MEM, "mem", "Dump kernel memory map"},
{ SH_MOUNT, "mount", "Mount the floppy"},
{ SH_PAGES, "pages", "Dump dirty pages"},
{ SH_PS, "ps", "Print the state of every processes"},
{ SH_READ, "read", "Read a block from the floppy to the buffer"},
{ SH_REBOOT, "reboot", "Reboot the system"},
{ SH_TEST, "test", "Create some auto-killing tasks"},
{ SH_UNAME, "uname", "Print kernel informations"},
{ SH_V86EXEC, "v86", "v86 mode unsupported"},
{ SH_WRITE, "write", "Write a block from the buffer to the floppy"},
{ SH_PWD, "pwd", "Print the current working directory"},
};


/**************************************************************************
* Used for multitasking debugging.
***************************************************************************/
void shell( s32int argc, char **argv){

	// Shell command buffer
	s08int cmd[256];
	s32int i;

	// Reset the command buffer
	memset08(cmd, 0, 256);

	// Command line
	while(TRUE) {
		kprintf(SHELL_PROMPT);

		scanf("%s", cmd);

		for(i = 0; i < SHELL_COMMANDS; i++) {
			// Parse the command
			if ( strncmp(cmd, commands[i].name, strlen(commands[i].name))==0 ) {
				if (*(cmd+strlen(commands[i].name)) == '\0') {
					*CMD_ARG = '\0';
						break;
				}
				if (*(cmd+strlen(commands[i].name)) == ' ') {
					break;
				}
			}
		}

		if (i == SHELL_COMMANDS) {
			// Command not found
			if (cmd[0]) {
				kprintf("\n\r%s: command not found!!!\n\r", cmd);
			}
			continue;
		}

		switch(commands[i].index){
					case SH_HELP:
						kprintf("\nBlueShell version 0.1 - by David Davidson\n");
						for (i = 0; i < SHELL_COMMANDS; i++) {
							kprintf("\n- %s : %s", commands[i].name, commands[i].help);
						}
						kprintf("\n\n");
						break;
					case SH_UNAME:
						kprintf("\n%s - Kernel [v%s]\n", KERNEL_NAME, KERNEL_VERSION);
						break;

					case SH_CLEAR:
						kclrscr();
						break;

					case SH_REBOOT:
						reboot();
						break;

					case SH_READ:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_WRITE:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_PAGES:
						dump_dirty_pages();
						break;

					case SH_MEM:
						dump_memory_map();
						break;

					case SH_CHECKMEM:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_FRAMES:
						dump_free_frames();
						break;

					case SH_EXEC:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_V86EXEC:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_TEST:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_CPUID:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_MOUNT:
						mount_floppy();
						break;

					case SH_LS:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_CAT:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_CD:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_PS:
						ps();
						break;

					case SH_KILL:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

					case SH_PWD:
						kprintf("%s command not yet Implemented.\n",cmd);
						break;

				}
	}
}
