#include "../commands/run.h"
#include "../commands/exec.h"
#include "../scheduling/ready_queue.h"

int run(char *script) {
    char *programs[MAX_NUM_PROGRAMS];
    programs[0] = script;
    return exec(programs, "FCFS");
}
