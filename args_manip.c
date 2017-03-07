/******************************************************************************
* File: args_manip.c
* Author: Gregory Chan
* Description: This file contains the functions ReadArgs() and PrintArgs() for
	       reading and printing arguments
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args_manip.h"

void ReadArgs(char *in, char **argv, int size)
{
	char *current_arg;
	int i = 0;	

	for (i = 0; i <= size; i++)
		argv[i] = NULL;

	i = 0;

	// loop through string until no more args are left or # exceeds size
	for (current_arg = strtok(in, " "); current_arg;
	     current_arg = strtok(NULL, " "))
	{
		if (i == size)
		{
			argv[i] = NULL;
			return;
		}
		
		// dealing with last arg having a newline
		if (current_arg[strlen(current_arg) - 1] == '\n')
			current_arg[strlen(current_arg) - 1] = '\0';

		// allocate memory and duplicate string
		argv[i] = strdup(current_arg);
		i++;
	}

	argv[i] = NULL;
}

void PrintArgs(char * const * const argv)
{
	int i;

	for (i = 0; argv[i] != NULL; i++)
	{
		printf("argv[%d] = '%s'\n", i, argv[i]);
	}
}
