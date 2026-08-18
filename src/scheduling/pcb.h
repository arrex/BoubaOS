#include "../core/interpreter.h"
#include "../memory/shellmemory.h"

// round up
#define MAX_PAGE_TABLE_SIZE (MAX_FILE_SIZE + FRAME_SIZE - 1) / FRAME_SIZE

typedef struct PCB {
    int pid;
    char* filename;
    char* file_contents[MAX_FILE_SIZE];
    int file_length;
    int page_table[MAX_PAGE_TABLE_SIZE];
    int program_counter;
    int job_score;
    struct PCB* next;
} PCB;

struct PCB* pcb_init(char* filename, char* file_contents[], int file_length);
struct PCB* pcb_dup_init(struct PCB* dup);
void pcb_deinit(struct PCB* pcb);
