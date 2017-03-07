/******************************************************************************
* File: command.c
* Author: Gregory Chan
* Description: This file contains functions regarding commands
******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "args_manip.h"
#include "command.h"
#include "process.h"

int exists_stdin_redirect(struct Command *, int);
int exists_stdout_redirect(struct Command *, int);
int exists_background(struct Command *, int);

void ReadCommand(char *line, struct Command *command)
{
	char *current_line;
	char *lines[MAX_SUB_COMMANDS];
	int i;

	// initialize subcommands to NULL
	for (i = 0; i < MAX_SUB_COMMANDS; i++)
		command->sub_commands[i].line = NULL;

	command->num_sub_commands = 0;
	i = 0;

	// Parse command into subcommands
	// Note: SubCommand.line may contain extra spaces and newlines
	for (current_line = strtok(line, "|"); current_line;
	     current_line = strtok(NULL, "|"))
	{
		if (i >= MAX_SUB_COMMANDS)
			break;

		lines[i] = strdup(current_line);	
		command->sub_commands[i].line = strdup(lines[i]);

		command->num_sub_commands += 1;

		i++;
	}	

	for (i = 0; i < command->num_sub_commands; i++)
	{
		ReadArgs(lines[i], command->sub_commands[i].argv, MAX_ARGS - 1);
	}

	command->stdin_redirect = NULL;
	command->stdout_redirect = NULL;
	command->background = 0;
}

void PrintCommand(struct Command const * const command)
{
	int i;
	
	for (i = 0; i < MAX_SUB_COMMANDS; i++)
	{
		if (command->sub_commands[i].line == NULL)
			break;

		/*
		printf("Command %d:\n", i);
		printf("Whole subcommand: \"%s\"\n", command->sub_commands[i].line);
		PrintArgs( command->sub_commands[i].argv);
		*/
	}

	printf("Redirect stdin: %s\n", command->stdin_redirect ? command->stdin_redirect : 
								 "No stdin redirect");
	printf("Redirect stdout: %s\n", command->stdout_redirect ? command->stdout_redirect :
								 "No stdout redirect");
	printf("Background: %s\n", command->background ? "yes" : "no");
}

void ReadRedirectsAndBackground(struct Command *command)
{
	exists_background(command, command->num_sub_commands - 1);
	exists_stdout_redirect(command, command->num_sub_commands - 1);
	exists_stdin_redirect(command, command->num_sub_commands - 1);
}

int RunCommand(struct Command const * const command)
{
	return runnprocess(command);
}

// this function takes a pointer to a command and a subcommand index
// and sets command->stdin_redirect if one exists and returns 1
int exists_stdin_redirect(struct Command *command, int index)
{
	assert(index < MAX_SUB_COMMANDS);

	int i;

	for (i = MAX_ARGS - 1; i >= 0; i--)
	{
		if (command->sub_commands[index].argv[i] == NULL)
			continue;

		if (command->sub_commands[index].argv[i][0] == '<')
		{
			// if a space is placed after '<'
			if (strlen(command->sub_commands[index].argv[i]) == 1)
			{
				command->stdin_redirect =
					strdup(command->sub_commands[index].argv[i + 1]);	
				
				command->sub_commands[index].argv[i] = NULL;
				command->sub_commands[index].argv[i + 1] = NULL;
			}

			// if no space is placed after '<'
			else
			{
				command->stdin_redirect =
					strdup(command->sub_commands[index].argv[i] + 1);

				command->sub_commands[index].argv[i] = NULL;
			}
			return 1;
		}
	}

	command->stdin_redirect = NULL;
	return 0;
}

// this function takes a pointer to a command and a subcommand index
// and sets command->stdout_redirect if one exists and returns 1
int exists_stdout_redirect(struct Command *command, int index)
{
	assert(index < MAX_SUB_COMMANDS);

	int i;

	for (i = MAX_ARGS - 1; i >= 0; i--)
	{
		if (command->sub_commands[index].argv[i] == NULL)
			continue;

		if (command->sub_commands[index].argv[i][0] == '>')
		{
			// if a space is placed after '<'
			if (strlen(command->sub_commands[index].argv[i]) == 1)
			{
				command->stdout_redirect =
					strdup(command->sub_commands[index].argv[i + 1]);	
				
				command->sub_commands[index].argv[i] = NULL;
				command->sub_commands[index].argv[i + 1] = NULL;
			}

			// if no space is placed after '<'
			else
			{
				command->stdout_redirect =
					strdup(command->sub_commands[index].argv[i] + 1);

				command->sub_commands[index].argv[i] = NULL;
			}
			return 1;
		}
	}

	command->stdin_redirect = NULL;
	return 0;
}

// this function takes a pointer to a command and a subcommand index
// and sets command->background if it exists and returns 1
int exists_background(struct Command *command, int index)
{
	assert(index < MAX_SUB_COMMANDS);

	int i;

	for (i = MAX_ARGS - 1; i >= 0; i--)
	{
		if (command->sub_commands[index].argv[i] == NULL)
			continue;
		if (command->sub_commands[index].argv[i][0] == '&')
		{
			command->sub_commands[index].argv[i] = NULL;
			command->background = 1;
			return 1;
		}
	}

	command->background = 0;
	return 0;
}
