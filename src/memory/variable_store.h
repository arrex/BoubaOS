#include "memory.h"

typedef struct var_table_entry {
    char* var;
    char* value;
} var_table_entry;

typedef struct variable_store {
    struct var_table_entry var_table[VAR_STORE_SIZE];
} variable_store;

struct variable_store* vstore_init();
char* vstore_get(struct variable_store* varmem, char* key);
void vstore_set(struct variable_store* varmem, char* var_in, char* value_in);
