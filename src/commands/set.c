#include "../commands/set.h"
#include "../memory/shellmemory.h"

int set(char *var, char *value) {
    set_var_value(var, value);
    return 0;
}
