#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/shell.h"
#include "../memory/lru.h"
#include "../memory/memory.h"
#include "ready_queue.h"

// First-come, first-server policy
// Execute each script in queue until completion sequentially
int scheduler_fcfs() {
    int errCode;

    while (get_process_count() > 0) {
        struct PCB* pcb = pop_process();

        while (pcb->program_counter < pcb->file_length) {
            int page = pcb->program_counter / FRAME_SIZE;
            int frame = pcb->page_table[page];

            if (frame == -1) {
                int free_frame = mem_find_available_frame();

                if (free_frame == -1) {
                    // choose trivial frame to evict, will implement LRU
                    // eviction in the future
                    int victim_frame = get_lru_frame();

                    printf("Page fault! Victim page contents:\n\n");
                    for (int address = victim_frame * FRAME_SIZE;
                         address < victim_frame * FRAME_SIZE + FRAME_SIZE;
                         address++) {
                        char* content = mem_get_fstore_value(address);

                        if (content == NULL) {
                            char null_content[] = "NULL";
                            content = null_content;
                        }

                        content[strcspn(content, "\n")] =
                            '\0';  // trim newline for formatting
                        printf("%s\n", content);
                    }
                    printf("\nEnd of victim page contents.\n");

                    mem_free_memory_frame(victim_frame);
                    mem_load_page_into_frame(pcb, page, victim_frame);
                } else {
                    printf("Page fault!\n");
                    mem_load_page_into_frame(pcb, page, free_frame);
                }

                continue;
            }

            access_frame(frame);
            int offset = pcb->program_counter % FRAME_SIZE;
            int address = frame * FRAME_SIZE + offset;

            errCode = parseInput(mem_get_fstore_value(address));
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

            int page = pcb->program_counter / FRAME_SIZE;
            int frame = pcb->page_table[page];

            if (frame == -1) {
                int free_frame = mem_find_available_frame();

                if (free_frame == -1) {
                    int victim_frame = get_lru_frame();

                    printf("Page fault! Victim page contents:\n\n");
                    for (int address = victim_frame * FRAME_SIZE;
                         address < victim_frame * FRAME_SIZE + FRAME_SIZE;
                         address++) {
                        char* content = mem_get_fstore_value(address);

                        if (content == NULL) {
                            char null_content[] = "NULL";
                            content = null_content;
                        }

                        content[strcspn(content, "\n")] =
                            '\0';  // trim newline for formatting
                        printf("%s\n", content);
                    }
                    printf("\nEnd of victim page contents.\n");

                    mem_free_memory_frame(victim_frame);
                    free_frame = victim_frame;
                    mem_load_page_into_frame(pcb, page, victim_frame);
                } else {
                    printf("Page fault!\n");
                    mem_load_page_into_frame(pcb, page, free_frame);
                }

                break;
            }

            access_frame(frame);
            int offset = pcb->program_counter % FRAME_SIZE;
            int address = frame * FRAME_SIZE + offset;

            errCode = parseInput(mem_get_fstore_value(address));
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
        int page = pcb->program_counter / FRAME_SIZE;
        int frame = pcb->page_table[page];

        if (frame == -1) {
            int free_frame = mem_find_available_frame();

            if (free_frame == -1) {
                // choose trivial frame to evict, will implement LRU eviction in
                // the future
                int victim_frame = get_lru_frame();

                printf("Page fault! Victim page contents:\n\n");
                for (int address = victim_frame * FRAME_SIZE;
                     address < victim_frame * FRAME_SIZE + FRAME_SIZE;
                     address++) {
                    char* content = mem_get_fstore_value(address);

                    if (content == NULL) {
                        char null_content[] = "NULL";
                        content = null_content;
                    }

                    content[strcspn(content, "\n")] =
                        '\0';  // trim newline for formatting
                    printf("%s\n", content);
                }
                printf("\nEnd of victim page contents.\n");

                mem_free_memory_frame(victim_frame);
                free_frame = victim_frame;
                mem_load_page_into_frame(pcb, page, victim_frame);
            } else {
                printf("Page fault!\n");
                mem_load_page_into_frame(pcb, page, free_frame);
            }

            frame = free_frame;
        }

        access_frame(frame);
        int offset = pcb->program_counter % FRAME_SIZE;
        int address = frame * FRAME_SIZE + offset;

        errCode = parseInput(mem_get_fstore_value(address));
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
