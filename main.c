#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

int main(void)
{
	int commandreturn;
	struct Command command;
	char s[200];

	while(1)
	{
		printf("$ ");
		fflush (stdout);

		fgets(s, sizeof s, stdin);

		ReadCommand(s, &command);
		ReadRedirectsAndBackground(&command);

		commandreturn = RunCommand(&command);
		
		if (commandreturn == -1)
			continue;

		if (command.background == 1)
			printf("[%d]\n", commandreturn);
	}

	return 0;
}
