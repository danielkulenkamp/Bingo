

#include "random_bag.h"

void swap(struct bag * b, int index1, int index2)
{
    int temp = b->buffer[index1];
    b->buffer[index1] = b->buffer[index2];
    b->buffer[index2] = temp;
}

void shuffle_array(struct bag * b)
{
    int seed = time(NULL);
    srand(seed);
    int iterations = rand() % 1000;

    for (int i = 0; i < iterations; i++)
    {
        seed += time(NULL);
        srand(seed++);
        int number1 = rand() % b->size;
        seed *= time(NULL);
        srand(seed++);
        int number2 = rand() % b->size;

        swap(b, number1, number2);

    }
    return;
}

void bag_init(int start_number, struct bag * b)
{
    if (b->initialized && start_number == b->last_start_number)
        return;

    b->current_index = 0;

    for (int i = 0, count = start_number; i < b->size; i++, count++)
        b->buffer[i] = count;

    #ifndef DEBUG
    shuffle_array(b);
    #endif
    b->initialized = true;
    b->last_start_number = start_number;

}

void bag_deinit(struct bag *b)
{
    free(b->buffer);
    free(b);
}
int get_number(int letter, struct bag * b) 
{
    bag_init(letter, b);

    int num = b->buffer[b->current_index];
    b->buffer[b->current_index] = -1;
    b->current_index += 1;
    return num;

}
