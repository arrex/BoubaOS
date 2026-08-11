#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>

#include "exec.h"
#include "../utils/badcommand.h"
#include "../scheduling/scheduler.h"
#include "../core/shell.h"
#include "../scheduling/ready_queue.h"

int copy_files_to_backing_store(char *programs[]);
int build_ready_queue(char *programs[]);

int exec(char *programs[], char *policy) {
    int errCode;

    errCode = copy_files_to_backing_store(programs);

    if (errCode != 0) {
        return errCode;
    }

    errCode = build_ready_queue(programs);

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

// === HELPER FUNCTIONS ===

/*
  Copies files to backing store folder. If many filenames passed in the programs array
  are identical, only one copy is made.
*/
int copy_files_to_backing_store(char *programs[]) {
    for (int i = 0; i < MAX_NUM_PROGRAMS; i++) {
        char *script = programs[i];

        // Copy scripts into backing store folder
        if (script != NULL) {
            // Buffers to hold source and destination filepath
            char source_path[256];
            char dest_path[256];
            // Get filepaths
            snprintf(source_path, sizeof(source_path), "%s", script);
            snprintf(dest_path, sizeof(dest_path), "./backing_store/%s", script);

            // If file already exists in backing store, then skip copying
            struct stat file_stats;
            if (stat(dest_path, &file_stats) == 0) {
                continue;
            }

            // Open source file for reading
            FILE *source = fopen(source_path, "rb");
            if (source == NULL) {
                return badcommandFileDoesNotExist();
            }

            // Open dest file for writing
            FILE *dest = fopen(dest_path, "wb");
            if (dest == NULL) {
                return badCommandErrorOccurred();
            }

            // Copy file contents from src to dest
            char buffer[MAX_USER_INPUT];
            size_t bytes;
            while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
                fwrite(buffer, 1, bytes, dest);
            }

            // Close files
            fclose(source);
            fclose(dest);
        }
    }

    return 0;
}

/*
  This helper function reads each file from the backing store and creates the appropriate amount
  of PCBs for each program and adds them to the ready queue.
*/
int build_ready_queue(char *programs[]) {
    for (int i = 0; i < MAX_NUM_PROGRAMS; i++) {
        char *script = programs[i];

        if (script != NULL) {
            // New PCB to be created
            struct PCB *pcb;
            // Potential PCB with same script name (for shared memory)
            struct PCB *duplicate = find_duplicate_script(script);

            // If duplicate found, we init PCB with same memory locations for memory sharing
            // Else we init a new PCB
            if (duplicate != NULL) {
                pcb = pcb_dup_init(duplicate);
            } else {
                char path[256];
                snprintf(path, sizeof(path), "backing_store/%s", script);
                FILE *p = fopen(path, "rt");

                if (p == NULL) {
                    return badcommandErrorReadingFromBackingStore();
                }

                // Buffer file contents in array
                char *file_contents[MAX_FILE_SIZE];
                char buffer[MAX_USER_INPUT];
                int line_count = 0;

                while (fgets(buffer, MAX_USER_INPUT, p) != NULL && line_count < MAX_FILE_SIZE) {
                    file_contents[line_count] = strdup(buffer);
                    line_count++;
                }

                fclose(p);

                // Init PCB struct for process... includes writing file contents to shell memory
                pcb = pcb_init(script, file_contents, line_count);
            }

            // Add PCB to ready queue
            append_process(pcb);
        }
    }

    return 0;
}
