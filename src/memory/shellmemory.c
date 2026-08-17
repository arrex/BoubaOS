#include "shellmemory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../scheduling/pcb.h"
#include "lru.h"

struct var_table_entry {
    char* var;
    char* value;
};

struct frame_table_entry {
    int page;
    struct PCB* pcb;
};

// Store a global variable table
struct var_table_entry vartable[VAR_STORE_SIZE];
// Shell memory will store string entries
char* shellmemory[FRAME_STORE_SIZE];
// Table tracking shell memory frame metadata
struct frame_table_entry frame_table[FRAME_STORE_SIZE / FRAME_SIZE];

/* === Shell memory functions === */
void mem_init() {
    for (int i = 0; i < FRAME_STORE_SIZE; i++) {
        shellmemory[i] = NULL;
    }

    for (int i = 0; i < VAR_STORE_SIZE; i++) {
        vartable[i].var = NULL;
        vartable[i].value = NULL;
    }

    for (int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++) {
        frame_table[i].page = -1;
        frame_table[i].pcb = NULL;
    }
}

// This function searches for a free frame in shell memory.
// Returns frame number if free frame is found, -1 otherwise.
int find_available_frame() {
    for (int frame = 0; frame < FRAME_STORE_SIZE / FRAME_SIZE; frame++) {
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
    if (address < 0 || address > FRAME_STORE_SIZE) {
        printf("Warning: address %d does not fall within frame store bounds\n",
               address);
        return;
    }

    shellmemory[address] = strdup(value_in);
}

char* mem_get_value(int address) {
    char* value = shellmemory[address];

    if (value != NULL) {
        return strdup(value);
    }

    return NULL;
}

void load_page_into_frame(struct PCB* pcb, int page, int frame) {
    if (page < 0 || page * FRAME_SIZE >= pcb->file_length) {
        printf("Warning: the page %d is not within range of file %s\n", page,
               pcb->filename);
        return;
    }

    for (int offset = 0; offset < FRAME_SIZE; offset++) {
        int line = page * FRAME_SIZE + offset;
        int address = frame * FRAME_SIZE + offset;

        if (line < pcb->file_length) {
            mem_set_value(pcb->file_contents[line], address);
        }
    }

    // upate frame table metadata
    frame_table[frame].page = page;
    frame_table[frame].pcb = pcb;

    // update pcb page table
    pcb->page_table[page] = frame;

    // update LRU ordering
    access_frame(frame);
}

void free_memory_frame(int frame) {
    int base = frame * FRAME_SIZE;
    for (int offset = 0; offset < FRAME_SIZE; offset++) {
        int address = base + offset;
        free(shellmemory[address]);
        shellmemory[address] = NULL;
    }

    // invalidate PCB page table entry
    struct PCB* pcb = frame_table[frame].pcb;
    int page = frame_table[frame].page;
    pcb->page_table[page] = -1;

    // invalidate frame table metadata to prevent staleness
    frame_table[frame].page = -1;
    frame_table[frame].pcb = NULL;
}

// This function either creates a new entry in the variable table
// or updates the current variable assignment.
// It also updates shell memory accordingly.
void set_var_value(char* var_in, char* value_in) {
    // Linear search variable table for given variable name
    for (int i = 0; i < VAR_STORE_SIZE; i++) {
        if (vartable[i].var != NULL && strcmp(vartable[i].var, var_in) == 0) {
            free(vartable[i].value);
            vartable[i].value = strdup(value_in);
            return;
        }
    }

    // Variable does not exist, need to find a free spot
    for (int i = 0; i < VAR_STORE_SIZE; i++) {
        if (vartable[i].var == NULL) {
            vartable[i].var = strdup(var_in);
            vartable[i].value = strdup(value_in);
            return;
        }
    }

    // Did not manage to insert into memory
    return;
}

// Get value based on input key
char* get_var_value(char* var_in) {
    for (int i = 0; i < VAR_STORE_SIZE; i++) {
        if (vartable[i].var != NULL && strcmp(vartable[i].var, var_in) == 0) {
            return strdup(vartable[i].value);
        }
    }

    // Case variable not found
    return NULL;
}
