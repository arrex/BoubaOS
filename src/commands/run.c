#include "../scheduling/ready_queue.h"
#include "commands.h"

int run(char* script) {
    char* programs[MAX_NUM_PROGRAMS];
    programs[0] = script;
    return exec(programs, "FCFS");
}
