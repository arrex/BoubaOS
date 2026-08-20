#include "../memory/memory.h"

int set(char* var, char* value) {
    mem_set_vstore_value(var, value);
    return 0;
}
