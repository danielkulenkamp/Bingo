#ifndef RANDOM_BAG_H
#define RANDOM_BAG_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

//#define DEBUG

struct bag {
    int current_index;
    int last_start_number;
    bool initialized;
    int size;
    int *buffer;
};


void swap(struct bag* b, int index1, int index2);
void shuffle_array(struct bag * b);
void bag_init(int start_number, struct bag * b);
void bag_deinit(struct bag *b);
int get_number(int letter, struct bag * b);


#endif // RANDOM_BAG_H