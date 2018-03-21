#include "queue.h"
#include <stdio.h>

int main() {
	int command;
	int my_pthread;
	queue *my_queue = createQueue();
	while(1) {
	scanf("%d", &command);
	if (command == 0) {
		scanf("%d", &my_pthread);
		printf("pthread to push: %d\n", my_pthread);
		pushQueue(my_queue, my_pthread);
		printf("size: %d, begin: %d, end: %d, isEmpty: %d\n", my_queue->size, my_queue->begin, my_queue->end, isEmptyQueue(*my_queue));
	}
	else {
		my_pthread = popQueue(my_queue);
		printf("popped pthread: %d\n", my_pthread);
		printf("size: %d, begin: %d, end: %d, isEmpty: %d\n", my_queue->size, my_queue->begin, my_queue->end, isEmptyQueue(*my_queue));
	}
}
	destroyQueue(my_queue);
}