/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * Source file for test with loops and a use-after-free flaw
 */
#include <stdlib.h>

int main(int argc, char* argv[]) {

    int* dynamic = malloc(sizeof(int));

    if (dynamic != NULL) {
        free(dynamic);
    }
    else {
        return 1;
    }

    return *dynamic;
}