#include <stdlib.h>
#include <sys/stat.h>

#include "../commands/my_mkdir.h"
#include "../utils/badcommand.h"
#include "../memory/shellmemory.h"
#include "../utils/utils.h"

int my_mkdir(char *dirname) {
    // Fetch from memory if preceded by '$' sign
    if (dirname[0] == '$') {
        dirname++;
        dirname = get_var_value(dirname);

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
