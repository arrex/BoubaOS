#include <stdio.h>

int help() {
    char help_string[] =
        "COMMAND          DESCRIPTION\n"
        "help             Displays all the commands\n"
        "quit             Exits / terminates the shell with \"Bye!\"\n"
        "set VAR STRING   Assigns a value to shell memory\n"
        "print VAR        Displays the STRING assigned to VAR\n"
        "run SCRIPT.TXT   Executes the file SCRIPT.TXT\n"
        "echo STRING      Displays STRING. If STRING starts with $, prints the "
        "value of the variable\n"
        "ls            Lists the contents of the current directory\n"
        "mkdir DIR     Creates a new directory with name DIR\n"
        "touch FILE    Creates a new empty file with name FILE\n"
        "cd DIR        Changes current directory to DIR\n"
        "fork CMD      Executes CMD with ARGS using fork-exec pattern\n";

    printf("%s\n", help_string);
    return 0;
}
