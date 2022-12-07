/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * Source file for simple test without dynamic memory flaws
 */

#include <stdlib.h>

int main(int argc, char* argv[]) {
    int* dynamic = malloc(sizeof(int));
    *dynamic = 1;

    free(dynamic);

    return *dynamic;
}
