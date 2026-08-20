#include <stdio.h>

#include "../memory/memory.h"
#include "../utils/badcommand.h"

int print(char* var) {
    char* memory_value = mem_get_vstore_value(var);

    if (memory_value == NULL) {
        return badcommandVariableDoesNotExist();
    }

    printf("%s\n", memory_value);
    return 0;
}
