#include <stdlib.h>
#include <string.h>

#include "../memory/shellmemory.h"
#include "../scheduling/ready_queue.h"

// Start PIDs at 1
static int next_pid = 1;

struct PCB* pcb_init(char* filename, char* file_contents[], int file_length) {
    struct PCB* pcb = malloc(sizeof(struct PCB));

    // Error allocating memory
    if (pcb == NULL) {
        return NULL;
    }

    pcb->pid = next_pid++;
    pcb->filename = strdup(filename);
    pcb->file_length = file_length;

    // init page table to map file frames to memory frames
    for (int file_frame = 0; file_frame < file_length;
         file_frame = file_frame + FRAME_SIZE) {
        int memory_frame = find_available_frame();

        for (int i = 0; i < FRAME_SIZE && file_frame + i < file_length; i++) {
            int file_line = file_frame + i;
            int memory_address = memory_frame * FRAME_SIZE + i;

            mem_set_value(file_contents[file_line], memory_address);
        }

        pcb->page_table[file_frame / FRAME_SIZE] = memory_frame;
    }

    pcb->program_counter = 0;
    // Our definition of job score will be the length of the file
    pcb->job_score = file_length;
    pcb->next = NULL;

    return pcb;
}

// This constructor is used whenever a duplicate PCB is found. We pass the
// addresses directly to enable memory sharing.
struct PCB* pcb_dup_init(struct PCB* dup) {
    struct PCB* pcb = malloc(sizeof(struct PCB));

    // Error allocating memory
    if (pcb == NULL) {
        return NULL;
    }

    pcb->pid = next_pid++;
    pcb->filename = strdup(dup->filename);
    pcb->file_length = dup->file_length;

    for (int i = 0; i < MAX_PAGE_TABLE_SIZE; i++) {
        pcb->page_table[i] = dup->page_table[i];
    }

    pcb->program_counter = 0;
    pcb->job_score = dup->job_score;
    pcb->next = NULL;

    return pcb;
}

// Frees all dynamically allocated memory for the PCB struct
// See typedef in pcd.h to get a better idea of what to free
void pcb_deinit(struct PCB* pcb) {
    if (pcb != NULL) {
        // Free memory entries allocated to file contents only if there
        // are no other PCBs sharing the memory
        if (find_duplicate_script(pcb->filename) == NULL) {
            for (int i = 0; i < MAX_PAGE_TABLE_SIZE; i++) {
                int frame = pcb->page_table[i];

                for (int offset = 0; offset < FRAME_SIZE; offset++) {
                    int address = frame * FRAME_SIZE + offset;
                    free_memory_entry(address);
                }
            }
        }

        // Free filename ptr
        free(pcb->filename);
        pcb->filename = NULL;
        // Free PCB ptr
        free(pcb);
        pcb = NULL;
    }
}
