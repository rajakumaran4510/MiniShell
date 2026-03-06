#include "header.h"

char prompt[50] = "MiniShell$: ";
char input_string[1024];

int main()
{
    system("clear");
    scan_input(prompt, input_string);
}
