#ifndef BINGO_H
#define BINGO_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "random_bag.h"

struct bingo_board board;

#define B_LOWER 1
#define B_UPPER 15
#define I_LOWER 16
#define I_UPPER 30
#define N_LOWER 31
#define N_UPPER 45
#define G_LOWER 46
#define G_UPPER 60
#define O_LOWER 61
#define O_UPPER 75

#define B 1
#define I 16
#define N 31
#define G 46
#define O 61

#define BOARD_SIZE 25
#define NUMBER_RANGE_INTERVAL 15
#define NUMBER_OF_BALLS 75

// #define TESTING


// used by players
struct bingo_board
{
    int board[BOARD_SIZE];
};

struct bingo_board generate_board();
void print_bingo_board(struct bingo_board);
bool mark_call(struct bingo_board *, int call);
bool check_bingo(struct bingo_board *);


// used by callers
struct bag *bingo_bag_init();
int call(struct bag *);




#endif // BINGO_H