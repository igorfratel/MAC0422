#include "queue.h"
#include <stdlib.h>

queue *createQueue()
{
    queue *my_queue;
    my_queue = malloc(sizeof(queue));
    my_queue->ids = malloc(QUEUE_SIZE * sizeof(int));
    my_queue->begin = 0;
    my_queue->end = 0;
    my_queue->size = QUEUE_SIZE;
    return my_queue;
}

void reallocQueue(queue *my_queue)
{
    int *aux_pointer = realloc(my_queue->ids, 2*(my_queue->size) * sizeof(int));
    if (aux_pointer == NULL) {       
        destroyQueue(my_queue);
        exit(0);
    }
    my_queue->ids = aux_pointer;
    my_queue->size *= 2;
}

int isEmptyQueue(queue my_queue)
{
    return my_queue.begin == my_queue.end;
}

void pushQueue(queue *my_queue, int my_id)
{
    if ((my_queue->end + 1) % my_queue->size == my_queue->begin)
        reallocQueue(my_queue);
    my_queue->ids[my_queue->end] = my_id;
    my_queue->end = (my_queue->end + 1) % my_queue->size;
}

int popQueue(queue *my_queue)
{
    int my_id;
    if (isEmptyQueue(*my_queue))
        return -1;
    my_id = my_queue->ids[my_queue->begin];
    my_queue->begin = (my_queue->begin + 1) % my_queue->size;
    return my_id;
}

void resetQueue(queue *my_queue)
{
    while (!isEmptyQueue(*my_queue))
        popQueue(my_queue);
}

void destroyQueue(queue *my_queue)
{
    free(my_queue->ids);
    free(my_queue);
}