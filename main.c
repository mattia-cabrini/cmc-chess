#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t chess_piece_t;

const chess_piece_t cpEMPTY  = 0;
const chess_piece_t cpPAWN   = 1;
const chess_piece_t cpROOK   = 2;
const chess_piece_t cpKNIGHT = 3;
const chess_piece_t cpBISHOP = 4;
const chess_piece_t cpQUEEN  = 5;
const chess_piece_t cpKING   = 6;

const char* cpWHITES         = ".PRNBQK";
const char* cpBLACKS         = ".prnbqk";

typedef struct chess_board_t
{
    chess_piece_t board[64];
}* chess_board;

static char chess_piece_to_char(chess_piece_t p);

static chess_piece_t chess_board_get_at(chess_board B, int row, int col);
static void          chess_board_init(chess_board B);
static void          chess_board_print(chess_board B);

int main(int argc, char** argv)
{
    struct chess_board_t board;

    (void)argc;
    (void)argv;

    chess_board_init(&board);
    chess_board_print(&board);

    exit(0);
}

static char chess_piece_to_char(chess_piece_t p)
{
    if (p > 6 || p < -6)
        return 'E';
    if (p > 0)
        return cpWHITES[p];
    else
        return cpBLACKS[-p];
}

static chess_piece_t chess_board_get_at(chess_board B, int row, int col)
{
    return B->board[8 * row + col];
}

static void chess_board_init(chess_board B)
{
    chess_piece_t V[] = {
        -cpROOK, -cpKNIGHT, -cpBISHOP, -cpKING, -cpQUEEN, -cpBISHOP, -cpKNIGHT,
        -cpROOK, -cpPAWN,   -cpPAWN,   -cpPAWN, -cpPAWN,  -cpPAWN,   -cpPAWN,
        -cpPAWN, -cpPAWN,   cpEMPTY,   cpEMPTY, cpEMPTY,  cpEMPTY,   cpEMPTY,
        cpEMPTY, cpEMPTY,   cpEMPTY,   cpEMPTY, cpEMPTY,  cpEMPTY,   cpEMPTY,
        cpEMPTY, cpEMPTY,   cpEMPTY,   cpEMPTY, cpEMPTY,  cpEMPTY,   cpEMPTY,
        cpEMPTY, cpEMPTY,   cpEMPTY,   cpEMPTY, cpEMPTY,  cpEMPTY,   cpEMPTY,
        cpEMPTY, cpEMPTY,   cpEMPTY,   cpEMPTY, cpEMPTY,  cpEMPTY,   cpPAWN,
        cpPAWN,  cpPAWN,    cpPAWN,    cpPAWN,  cpPAWN,   cpPAWN,    cpPAWN,
        cpROOK,  cpKNIGHT,  cpBISHOP,  cpKING,  cpQUEEN,  cpBISHOP,  cpKNIGHT,
        cpROOK
    };

    memcpy(B->board, V, sizeof(V));
}

static void chess_board_print(chess_board B)
{
    int r;
    int c;

    printf("    ");
    for (c = 0; c < 8; ++c)
        printf("%c ", 'A' + c);
    printf("\n");

    for (r = 0; r < 8; ++r)
    {
        printf("\n%d   ", r + 1);
        for (c = 0; c < 8; ++c)
        {
            chess_piece_t p = chess_board_get_at(B, r, c);
            printf("%c ", chess_piece_to_char(p));
        }
    }

    printf("\n\n");
}
