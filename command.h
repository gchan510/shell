#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_ARGS 10
#define MAX_SUB_COMMANDS 5

struct SubCommand
{
	char *line;
	char *argv[MAX_ARGS];
};

struct Command
{
	struct SubCommand sub_commands[MAX_SUB_COMMANDS];
	int num_sub_commands;
	char *stdin_redirect;
	char *stdout_redirect;
	int background;
};

void ReadCommand(char *, struct Command *);
void PrintCommand(struct Command const * const);
void ReadRedirectsAndBackground(struct Command *);

int RunCommand(struct Command const * const); 

#endif // COMMANDS_H
