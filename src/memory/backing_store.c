#include <unistd.h>
#include <stdlib.h>

#include "../core/interpreter.h"
#include "../memory/backing_store.h"
#include "../commands/my_mkdir.h"

// Creates a backing store -> part of disk that is used by paging system to store information
// not currently in main memory...
// Upon shell init, creates a new backing store
// If backing store already exists (because of abnormal termination), it is overridden
// Backing store is deleted when quit command is called
void backing_store_init() {
    // Folder exists
    if (access("backing_store", F_OK) == 0) {
        backing_store_deinit();
    }

    // Create new backing store
    my_mkdir("backing_store");
}

// Removes existing backing store
void backing_store_deinit() {
    system("rm -rf backing_store");
}
