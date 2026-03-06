#include "header.h"

char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
					"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
					"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "fg", "jobs", "bg", NULL};

char cmd[10];
char *ext_cmds[200];
int pid = 0;
int status;
node *head = NULL;

char *get_command(char *input_string)
{
	int i = 0;
	while (input_string[i] != ' ' && input_string[i] != '\0')
	{
		i++;
	}
	input_string[i] = '\0';
	return input_string;
}

int check_cmd_type(char *cmd)
{
	for (int i = 0; builtins[i] != NULL; i++)
	{
		if (strcmp(cmd, builtins[i]) == 0)
			return BUILTIN;
	}
	return EXTERNAL;
}

void execute_internal_commands(char *input_string)
{
	if (strcmp(input_string, "exit") == 0)
	{
		exit(0);
	}
	else if (strcmp(input_string, "pwd") == 0)
	{
		char buffer[50];
		getcwd(buffer, 50);
		printf("%s\n", buffer);
	}
	else if (strncmp(input_string, "cd", 2) == 0)
	{
		char buffer[50];
		chdir(input_string + 3);
		{
			printf("%s\n", getcwd(buffer, 50));
		}
	}
	else if (strcmp(input_string, "echo $$") == 0)
	{
		printf("%d\n", getpid());
	}
	else if (strcmp(input_string, "echo $?") == 0)
	{
		if (WIFEXITED(status))
		{
			printf("%d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			printf("%d\n", 128 + WTERMSIG(status));
		}
	}
	else if (strcmp(input_string, "echo $SHELL") == 0)
	{
		printf("%s\n", getenv("SHELL"));
	}
	else if (strcmp(input_string, "jobs") == 0)
	{
		print_list();
	}
	else if (strcmp(input_string, "fg") == 0)
	{
		if (head == NULL)
		{
			printf("fg: current: no such job\n");
			return;
		}
		kill(head->pid, SIGCONT);
		waitpid(head->pid, &status, WUNTRACED);
		delete_first();
	}
	else if (strcmp(input_string, "bg") == 0)
	{
		if (head == NULL)
		{
			printf("bg: current: no such job\n");
			return;
		}
		signal(SIGCHLD, signal_handler);
		kill(head->pid, SIGCONT);
	}
}

void execute_external_commands(char *input_string)
{
	char *argv[50];
	int argc = 0;
	char *token = strtok(input_string, " ");
	while (token != NULL)
	{
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}
	argv[argc] = NULL;
	int cmd[20], cmd_count = 0;
	cmd[cmd_count++] = 0;
	int pipe_present = 0;

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "|") == 0)
		{
			argv[i] = NULL;
			cmd[cmd_count++] = i + 1;
			pipe_present = 1;
		}
	}
	if (pipe_present == 0)
	{
		pid = fork();
		if (pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			execvp(argv[0], argv);
			printf("%s: command not found\n", argv[0]);
			exit(1);
		}
		else
		{
			waitpid(pid, &status, WUNTRACED);
			pid = 0;
		}
		return;
	}
	int fd[2];
	int prev_fd = -1;
	for (int i = 0; i < cmd_count; i++)
	{
		if (i < cmd_count - 1)
		{
			if (pipe(fd) == -1)
			{
				perror("pipe");
				exit(1);
			}
		}
		int pid = fork();
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, 0);
				close(prev_fd);
			}
			if (i < cmd_count - 1)
			{
				close(fd[0]);
				dup2(fd[1], 1);
				close(fd[1]);
			}
			execvp(argv[cmd[i]], &argv[cmd[i]]);
			perror("Command not found");
			exit(1);
		}
		if (prev_fd != -1)
			close(prev_fd);

		if (i < cmd_count - 1)
		{
			close(fd[1]);
			prev_fd = fd[0];
		}
	}
	for (int i = 0; i < cmd_count; i++)
	{
		wait(NULL);
	}
}
