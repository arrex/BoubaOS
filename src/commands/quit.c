#include <stdio.h>
#include <stdlib.h>

#include "../commands/quit.h"
#include "../memory/backing_store.h"

int quit() {
    backing_store_deinit();
    printf("Bye!\n");
    exit(0);
}
