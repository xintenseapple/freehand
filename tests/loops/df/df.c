/* FreeHand Dynamic Memory Safety Analysis Tool
 * Author: Evan Hellman
 * Source file for test with loops and a double-free flaw
 */
#include <stdlib.h>

int main(int argc, char* argv[]) {

    unsigned int** matrix = malloc(sizeof(unsigned int) * 10);

    for (unsigned int i = 0; i < 10; i++) {
        matrix[i] = malloc(sizeof(unsigned int) * 10);
    }

    unsigned int counter = 0;

    for (unsigned int i = 0; i < 10; i++) {
        for (unsigned int j = 0; i < 10; i++) {
            matrix[i][j] = counter++;
        }
    }

    int r = matrix[9][9];

    free(matrix[0]);
    for (unsigned int i = 0; i < 10; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return r;
}