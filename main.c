#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct chess_board_t
{
    uint64_t board;
}* chess_board;

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    printf("Hello, World!\n");

    exit(0);
}
