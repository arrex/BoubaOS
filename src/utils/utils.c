#include <ctype.h>
#include <stdlib.h>

/*
  Checks if a string is valid. Used for checking file and directory names.
  Alphanumeric characters and underscores are allowed.
  Returns:
  - 0 if not alphanumeric
  - 1 if alphanumeric
*/
int is_valid_name(char* string) {
    if (string == NULL) {
        return 0;
    }

    int ix = 0;
    while (string[ix] != '\0') {
        if (isalnum(string[ix]) == 0 && string[ix] != '_') {
            return 0;
        }

        ix++;
    }

    return 1;
}
