#include "header.h"

void insert_at_first(int pid, char *cmd)
{
    node *new = malloc(sizeof(node));
    if (new == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    new->pid = pid;
    strcpy(new->command, cmd);
    new->next = head;
    head = new;
}

void delete_first()
{
    if (head == NULL)
    {
        return;
    }
    node *temp = head;
    head = head->next;
    free(temp);
}

void print_list()
{
    node *temp = head;
    int job_no = 1;
    if (temp == NULL)
    {
        printf("No jobs\n");
        return;
    }
    while (temp != NULL)
    {
        printf("[%d] PID : %d CMD : %s\n", job_no, temp->pid, temp->command);
        temp = temp->next;
        job_no++;
    }
}
