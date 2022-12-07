/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * Source file for simple test with a use-after-free flaw
 */

#include <stdlib.h>

int main(int argc, char* argv[]) {
    int* dynamic = malloc(sizeof(int));
    *dynamic = 1;

    free(dynamic);

    return *dynamic;
}
