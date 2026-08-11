#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../core/shell.h"
#include "../scheduling/ready_queue.h"
#include "../scheduling/scheduler.h"
#include "../utils/badcommand.h"

int build_ready_queue(char* programs[]);

int exec(char* programs[], char* policy) {
    int errCode = build_ready_queue(programs);

    if (errCode != 0) {
        return errCode;
    }

    if (strcmp(policy, "FCFS") == 0) {
        return scheduler_fcfs();
    } else if (strcmp(policy, "SJF") == 0) {
        return scheduler_sjf();
    } else if (strcmp(policy, "RR") == 0) {
        return scheduler_rr(RR_TIMESLICE);
    } else if (strcmp(policy, "RR30") == 0) {
        return scheduler_rr(RR30_TIMESLICE);
    } else if (strcmp(policy, "AGING") == 0) {
        return scheduler_aging();
    } else {
        return badcommandInvalidPolicy();
    }
}

/*
  This helper function reads each file from the backing store and creates the
  appropriate amount of PCBs for each program and adds them to the ready queue.
*/
int build_ready_queue(char* programs[]) {
    for (int i = 0; i < MAX_NUM_PROGRAMS; i++) {
        char* script = programs[i];

        if (script != NULL) {
            // New PCB to be created
            struct PCB* pcb;
            // Potential PCB with same script name (for shared memory)
            struct PCB* duplicate = find_duplicate_script(script);

            // If duplicate found, we init PCB with same memory locations for
            // memory sharing Else we init a new PCB
            if (duplicate != NULL) {
                pcb = pcb_dup_init(duplicate);
            } else {
                char path[256];
                snprintf(path, sizeof(path), "%s", script);
                FILE* p = fopen(path, "rt");

                if (p == NULL) {
                    return badcommandFileDoesNotExist();
                }

                // Buffer file contents in array
                char* file_contents[MAX_FILE_SIZE];
                char buffer[MAX_USER_INPUT];
                int line_count = 0;

                while (fgets(buffer, MAX_USER_INPUT, p) != NULL &&
                       line_count < MAX_FILE_SIZE) {
                    file_contents[line_count] = strdup(buffer);
                    line_count++;
                }

                fclose(p);

                // Init PCB struct for process... includes writing file contents
                // to shell memory
                pcb = pcb_init(script, file_contents, line_count);
            }

            // Add PCB to ready queue
            append_process(pcb);
        }
    }

    return 0;
}
