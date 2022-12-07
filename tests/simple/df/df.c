/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * Source file for simple test with a double-free flaw
 */

#include <stdlib.h>

int main(int argc, char* argv[]) {
    int* dynamic = malloc(sizeof(int));
    *dynamic = 1;

    int* dynamic2 = dynamic;

    free(dynamic);
    free(dynamic2);

    return 0;
}
