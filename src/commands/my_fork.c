#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../utils/badcommand.h"

int my_fork(char** args, int args_size) {
    // Returns 0 in child, positive value in parent
    pid_t pid = fork();

    // Fork failed
    if (pid == -1) {
        return badCommandErrorOccurred();
    }

    if (pid == 0) {
        // Child
        char* fork_args[args_size + 1];

        for (int i = 0; i < args_size; i++) {
            fork_args[i] = args[i];
        }

        // NULL terminate array of arguments for execvp
        fork_args[args_size] = NULL;

        execvp(fork_args[0], fork_args);

        exit(1);
    } else {
        // Parent
        int status;
        // Wait for child to complete
        waitpid(pid, &status, 0);

        return WEXITSTATUS(status);
    }
}
