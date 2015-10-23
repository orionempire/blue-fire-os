/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: task.c
 *	Created: Oct 22, 2015
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Current running task
task_t	*curr_task = NULL;

int get_pid() {
	// Return the current task pid if there is a curr_task
	if (!curr_task) return NULL;
	return(curr_task->pid);
}
