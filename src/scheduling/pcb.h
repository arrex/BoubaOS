#include "../core/interpreter.h"
#include "../memory/shellmemory.h"

#define MAX_PAGE_TABLE_SIZE ((MAX_FILE_SIZE / FRAME_SIZE) + 1)

typedef struct PCB {
    int pid;
    char* filename;
    int file_length;
    int addresses[MAX_FILE_SIZE];
    int program_counter;
    int job_score;
    int page_table[MAX_PAGE_TABLE_SIZE];
    struct PCB* next;
} PCB;

struct PCB* pcb_init(char* filename, char* file_contents[], int file_length);
struct PCB* pcb_dup_init(struct PCB* dup);
void pcb_deinit(struct PCB* pcb);
