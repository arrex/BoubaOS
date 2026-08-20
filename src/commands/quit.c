#include "../memory/memory.h"

#include <stdio.h>
#include <stdlib.h>

int quit() {
    printf("Bye!\n");
    mem_deinit();
    exit(0);
}
