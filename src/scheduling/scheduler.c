#include "../core/shell.h"
#include "../memory/shellmemory.h"
#include "../scheduling/ready_queue.h"

// First-come, first-server policy
// Execute each script in queue until completion sequentially
int scheduler_fcfs() {
    int errCode;

    while (get_process_count() > 0) {
        struct PCB* pcb = pop_process();

        while (pcb->program_counter < pcb->file_length) {
            int address = pcb->addresses[pcb->program_counter];
            errCode = parseInput(mem_get_value(address));
            pcb->program_counter++;
        }

        // Cleanup
        pcb_deinit(pcb);
    }

    return errCode;
}

int scheduler_sjf() {
    // Sort PCBs in ready queue by job score
    sort_ready_queue();
    // Execution order is same as FCFS now since sorted...
    return scheduler_fcfs();
}

int scheduler_rr(int time_slice) {
    int errCode;

    while (get_process_count() > 0) {
        struct PCB* pcb = pop_process();

        for (int i = 0; i < time_slice; i++) {
            // Check if exhausted lines to execute
            if (pcb->program_counter == pcb->file_length) {
                break;
            }

            int address = pcb->addresses[pcb->program_counter];
            errCode = parseInput(mem_get_value(address));
            pcb->program_counter++;
        }

        // Add PCB back to queue if still instructions to execute
        if (pcb->program_counter == pcb->file_length) {
            pcb_deinit(pcb);
        } else {
            append_process(pcb);
        }
    }

    return errCode;
}

int scheduler_aging() {
    int errCode;

    // Sort PCBs in ready queue by job score
    sort_ready_queue();

    while (get_process_count() > 0) {
        // Get element at head of ready queue
        struct PCB* pcb = pop_process();

        // Execute next instruction
        int address = pcb->addresses[pcb->program_counter];
        errCode = parseInput(mem_get_value(address));
        pcb->program_counter++;

        // Check if process is complete
        if (pcb->program_counter == pcb->file_length) {
            pcb_deinit(pcb);
        } else {
            // Age ready queue
            age_ready_queue();

            // Add PCB back to queue at head position
            // We insert at head to resolve tie-breaking and guarantee that
            // current PCB has priority
            appendleft_process(pcb);

            // Sort queue after aging
            sort_ready_queue();
        }
    }

    return errCode;
}
