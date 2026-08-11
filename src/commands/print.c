#include <stdio.h>

#include "../commands/print.h"
#include "../memory/shellmemory.h"
#include "../utils/badcommand.h"

int print(char *var) {
    char *memory_value = get_var_value(var);

    if (memory_value == NULL) {
        return badcommandVariableDoesNotExist();
    }

    printf("%s\n", memory_value);
    return 0;
}
