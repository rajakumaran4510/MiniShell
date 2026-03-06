#include "header.h"

extern char prompt[50];
extern char input_string[1024];
extern int pid, status;
int job = 0;

int check_space(char *str)
{
    int i = 4;
    if (str[i] == '\0')
        return 0;
    while (str[i] != '\0')
    {
        if (str[i] == ' ')
        {
            return 0;
        }
        i++;
    }
    return 1;
}

void signal_handler(int sig_num)
{
    if (sig_num == SIGINT)
    {
        if (pid == 0)
            printf(ANSI_COLOR_DARK_GREEN "\n%s\n" ANSI_COLOR_RESET, prompt);
    }

    if (sig_num == SIGTSTP)
    {
        if (pid == 0)
            printf(ANSI_COLOR_DARK_GREEN "\n%s\n" ANSI_COLOR_RESET, prompt);
        else if (pid != 0)
        {
            insert_at_first(pid, input_string);
            job++;
            printf("\n[%d]+ Stopped    %s\n", job, input_string);
            pid = 0;
        }
    }
    if (sig_num == SIGCHLD)
    {
        int status;
        if (waitpid(-1, &status, WNOHANG) > 0)
        {
            delete_first();
        }
    }
}

void scan_input(char *prompt, char *input_string)
{
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    while (1)
    {
        printf(ANSI_COLOR_DARK_GREEN "%s" ANSI_COLOR_RESET, prompt);
        fflush(stdout);
        if (fgets(input_string, 1024, stdin) == NULL)
            continue;
        input_string[strcspn(input_string, "\n")] = 0;
        if (strlen(input_string) == 0)
            continue;
        if (strncmp(input_string, "PS1=", 4) == 0)
        {
            if (check_space(input_string))
            {
                strcpy(prompt, input_string + 4);
            }
        }
        else
        {
            char input_copy[1024];
            strcpy(input_copy, input_string);
            char *cmd = get_command(input_copy);
            int type = check_cmd_type(cmd);
            if (type == BUILTIN)
                execute_internal_commands(input_string);
            else if (type == EXTERNAL)
            {
                execute_external_commands(input_string);
            }
        }
    }
}
