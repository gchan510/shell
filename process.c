#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process.h"

int pipesuccess(int pipeerr);
int procsuccess(int procstatus);

// This process runs the (index)th subcommand from command and redirects
// input and output accordingly
// Return values: -1 if error
// 	          child pid otherwise
int run1process(struct Command const * const command, int const index,
		int const read_end, int const write_end)
{
	int openstatus;
	int forkret = fork();

	if (forkret < 0)
	{
		printf("Error creating process\n");
		return -1;
	}
	
	if (forkret == 0)
	{
		// stdin redirect
		if (index == 0)
		{
			if (command->stdin_redirect != NULL)
			{
				close(0);
				openstatus = open(command->stdin_redirect,
						  O_RDONLY);
				if (openstatus < 0)
				{
					printf("Error opening file: %s\n",
					       command->stdin_redirect);
					exit(1);
				}
			}
		}

		// stdout redirect
		if (index == command->num_sub_commands - 1)
		{
			if (command->stdout_redirect != NULL)
			{
				close(1);
				openstatus = open(command->stdout_redirect,
						  O_WRONLY | O_CREAT, 0660);
				if (openstatus < 0)
				{
					printf("Error opening file: %s\n",
					       command->stdout_redirect);
					exit(1);
				}
			}
		}

		// pipe input
		if (read_end != -1)
		{
			close(0);
			dup(read_end);
		}

		// pipe output
		if (write_end != -1)
		{
			close(1);
			dup(write_end);
		}

		if (execvp(command->sub_commands[index].argv[0],
			   command->sub_commands[index].argv) == -1)
		{
			// error executing command, exit child process
			printf("%s: command not found\n",
			       command->sub_commands[index].argv[0]);
			exit(1);
		}
	}

	else
	{
		close(read_end);
		close(write_end);
		if (command->background == 0)
		{
			// wait for the last subcommand
			if (index == command->num_sub_commands - 1)
			{
				errno = 0;
				waitpid(forkret, NULL, 0);

				if (errno == 0)
					return forkret;
				else
					return -1;
			}
			// otherwise return immediately
			else
				return forkret;
		}
		else
		{
			return forkret;
		}
	}	

	printf("Error in run1process()\n");
	return -1;
}

// This function runs the subcommands from command
// Return values: -1 on error
// 		  pid of last subcommand otherewise
int runnprocess(struct Command const * const command)
{
	int i;
	int pipeerr, procstatus;

	int fds[command->num_sub_commands - 1][2];

	// create pipes
	for (i = 0; i < command->num_sub_commands - 1; i++)
	{
		pipeerr = pipe(fds[i]);
		if (pipesuccess(pipeerr) == 0)
			return -1;
	}

	// if only one subcommand, no pipes
	if (command->num_sub_commands == 1)
	{
		return run1process(command, 0, -1, -1);
	}

	for (i = 0; i < command->num_sub_commands; i++)
	{
		// first subcommand has no pipe in
		if (i == 0)
		{
			procstatus = run1process(command, i, -1, fds[i][1]);
			if (procsuccess(procstatus) == 0)
				goto cleanup;
		}

		// last subcommand has no pipe out
		else if (i == command->num_sub_commands - 1)
		{
			procstatus = run1process(command, i, fds[i - 1][0], -1);
			if (procsuccess(procstatus) == 0)
				goto cleanup;
			return procstatus;
		}

		else
		{
			procstatus = run1process(command, i, fds[i - 1][0], fds[i][1]);
			if (procsuccess(procstatus) == 0)
				goto cleanup;
		}

		// close unused pipes
		if (i != 0)
		{
			close(fds[i - 1][0]);
			close(fds[i - 1][1]);
		}
	}
	
cleanup:
	while (i < command->num_sub_commands)
	{
		close(fds[i - 1][0]);
		close(fds[i - 1][1]);
		i++;
	}

	return -1;
}

int pipesuccess(int pipeerr)
{
	if (pipeerr < 0)
	{
		printf("Error creating pipe\n");
		return 0;
	}

	return 1;
}

int procsuccess(int procstatus)
{
	if (procstatus < 0)
	{
		printf("Error running process\n");
		return 0;
	}

	return 1;
}
