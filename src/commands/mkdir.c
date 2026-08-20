#include "mkdir.h"

#include <stdlib.h>
#include <sys/stat.h>

#include "../memory/memory.h"
#include "../utils/badcommand.h"
#include "../utils/utils.h"

const int mmkdir(char* dirname) {
    // Fetch from memory if preceded by '$' sign
    if (dirname[0] == '$') {
        dirname++;
        dirname = mem_get_vstore_value(dirname);

        if (dirname == NULL) {
            return badcommandVariableDoesNotExist();
        }
    }

    if (is_valid_name(dirname) != 1) {
        return badcommandNameNotAlphanum();
    }

    // Error creating directory
    if (mkdir(dirname, DIRECTORY_PERMS) == -1) {
        return badCommandErrorOccurred();
    }

    return 0;
}
