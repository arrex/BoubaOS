#include "../memory/shellmemory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct var_table_entry {
    char* var;
    char* value;
};

// Store a global variable table
struct var_table_entry vartable[VAR_TABLE_SIZE];
// Shell memory will store string entries
char* shellmemory[SHELL_MEM_SIZE];

/* === Shell memory functions === */
void mem_init() {
    for (int i = 0; i < SHELL_MEM_SIZE; i++) {
        shellmemory[i] = NULL;
    }

    for (int i = 0; i < VAR_TABLE_SIZE; i++) {
        vartable[i].var = NULL;
        vartable[i].value = NULL;
    }
}

// This function searches for a free frame in shell memory.
// Returns frame number if free frame is found, -1 otherwise.
int find_available_frame() {
    for (int frame = 0; frame < SHELL_MEM_SIZE / FRAME_SIZE; frame++) {
        int address = frame * FRAME_SIZE;

        if (shellmemory[address] == NULL) {
            return frame;
        }
    }

    // No free frame found
    return -1;
}

// This function inserts a new entry into shell memory.
void mem_set_value(char* value_in, int address) {
    if (address < 0 || address > SHELL_MEM_SIZE) {
        return;
    }

    shellmemory[address] = strdup(value_in);
}

char* mem_get_value(int address) { return strdup(shellmemory[address]); }

void free_memory_entry(int address) {
    if (address >= 0 && address < SHELL_MEM_SIZE &&
        shellmemory[address] != NULL) {
        free(shellmemory[address]);
        shellmemory[address] = NULL;
    }
}

// This function either creates a new entry in the variable table
// or updates the current variable assignment.
// It also updates shell memory accordingly.
void set_var_value(char* var_in, char* value_in) {
    // Linear search variable table for given variable name
    for (int i = 0; i < VAR_TABLE_SIZE; i++) {
        if (vartable[i].var != NULL && strcmp(vartable[i].var, var_in) == 0) {
            free(vartable[i].value);
            vartable[i].value = strdup(value_in);
            return;
        }
    }

    // Variable does not exist, need to find a free spot
    for (int i = 0; i < VAR_TABLE_SIZE; i++) {
        if (vartable[i].var == NULL) {
            vartable[i].var = strdup(var_in);
            vartable[i].value = strdup(value_in);
            return;
        }
    }

    // Did not manager to insert into memory
    return;
}

// Get value based on input key
char* get_var_value(char* var_in) {
    for (int i = 0; i < VAR_TABLE_SIZE; i++) {
        if (vartable[i].var != NULL && strcmp(vartable[i].var, var_in) == 0) {
            return strdup(vartable[i].value);
        }
    }

    // Case variable not found
    return NULL;
}
