
#include "bingo.h"


struct bingo_board generate_board() 
{
    struct bingo_board b;
    struct bag *random = malloc(sizeof(struct bag));
    random->buffer = malloc(NUMBER_RANGE_INTERVAL * sizeof(int));
    random->size = NUMBER_RANGE_INTERVAL;
    random->initialized = false;
    random->current_index = 0;
    random->last_start_number = -1;

    for (int i = 0; i < 25; i++)
    {
        if (i < 5)
            b.board[i] = get_number(B, random);
        else if (i < 10)
            b.board[i] = get_number(I, random);
        else if (i < 15)
        {
            if (i == 12)
                b.board[i] = -1;
            else
                b.board[i] = get_number(N, random);
        }
        else if (i < 20)
            b.board[i] = get_number(G, random);
        else if (i < 25)
            b.board[i] = get_number(O, random);
    }

    bag_deinit(random);

    return b;
}

void print_bingo_board(struct bingo_board board) 
{
    printf("-------------------------------\n");
    printf("|  B     I     N     G     O  |\n");
    printf("|-----------------------------|\n");
    for (int i = 0; i < 5; i++)
    {
        char letter;
        for (int j = i; j < 25; j+=5)
        {
            if (j < 5)
                letter = 'B';
            else if (j < 10)
                letter = 'I';
            else if (j < 15) 
                letter = 'N';
            else if (j < 20)
                letter = 'G';
            else if (j < 25)
                letter = 'O';


            if (board.board[j] == -1)
                printf("   X  ");
            else if (j == i)
                printf("| %c%2d ", letter, board.board[j]);
            else if (j >= 20)
                printf("  %c%2d |", letter, board.board[j]);
            else
                printf("  %c%2d ", letter, board.board[j]);
        }  
        if (i == 4)
            printf("\n-------------------------------\n");
        else
            printf("\n|                             |\n");
    }
}

bool mark_call(struct bingo_board *card, int call)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (card->board[i] == call)
        {
            card->board[i] = -1;
            break;
        }
    }

    if (check_bingo(card))
        return true;
    else
        return false;

}

bool check_bingo(struct bingo_board *card)
{
    // check five columns
    for (int i = 0; i < 5; i++)
    {
        int offset = 5 * i;
        if (card->board[offset + 0] == card->board[offset + 1] &&
            card->board[offset + 1] == card->board[offset + 2] &&
            card->board[offset + 2] == card->board[offset + 3] &&
            card->board[offset + 3 ]== card->board[offset + 4] &&
            card->board[offset + 4] == -1)
        {
            return true;
        }
    }
    // check five rows
    for (int i = 0; i < 5; i++)
    {
        //int offset = 0;
        if (card->board[i + 0] == card->board[i + 5] &&
            card->board[i + 5] == card->board[i + 10] &&
            card->board[i + 10] == card->board[i + 15] &&
            card->board[i + 15] == card->board[i+20] &&
            card->board[i + 20] == -1)
        {
            return true;
        }
    }
    // check two diagonals
    if (card->board[0] == card->board[6] &&
        card->board[6] == card->board[12] &&
        card->board[12] == card->board[18] &&
        card->board[18] == card->board[24] &&
        card->board[24] == -1)
    {
        return true;
    }

    if (card->board[20] == card->board[16] &&
        card->board[16] == card->board[12] &&
        card->board[12] == card->board[8] &&
        card->board[8] == card->board[4] &&
        card->board[4] == -1)
    {
        return true;
    }

    // if it gets here, there isn't a bingo
    return false;
    
}

// make sure to call bag_deinit(struct bag*) on this when you are done
struct bag *bingo_bag_init() 
{
    struct bag *bingo_bag = malloc(sizeof(struct bag));
    bingo_bag->buffer = malloc(sizeof(int) * NUMBER_OF_BALLS);
    bingo_bag->size = NUMBER_OF_BALLS;
    bingo_bag->current_index = 0;
    bingo_bag->initialized = false;
    bingo_bag->last_start_number = 1;
    bag_init(B_LOWER, bingo_bag);

    return bingo_bag;
}


int call(struct bag * b) 
{
    return get_number(B_LOWER, b);
}


// #ifdef TESTING
// int main()
// {
//     struct bingo_board b = generate_board();

//     print_bingo_board(b);

//     struct bag *bingo_bag;
//     bingo_bag = bingo_bag_init();

//     while (!check_bingo(&b))
//     {
//         int ca = call(bingo_bag);
//         printf("CALL: %d\n", ca);
//         mark_call(&b, ca);
//         print_bingo_board(b);
//     }
//     printf("BINGO!\n");
//     print_bingo_board(b);

//     bag_deinit(bingo_bag);
// }
// #endif