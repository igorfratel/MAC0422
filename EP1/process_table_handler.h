#ifndef PROCESS_TABLE_HANDLER_H_
#define PROCESS_TABLE_HANDLER_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "process.h"

#define QUANTUM 0.1

extern process_table p_table;
extern pthread_mutex_t mutex_ptable;

/*Orders first by status in descending order. If status is equal, orders by dt in ascending order*/
int compare_dt(const void *a, const void *b);

/*Sorts the extern global variable process_table in ascending order according to given parameter*/
void sort_table();

/*Allocates the process_table with one process. Returns 1 in case of error and 0 otherwise*/
int emalloc_table();

/*Creates a new table with new_size * p_table.size * sizeof(process)
	 *and assigns the process_table pointers to this new table.
	 *Removes all executed processes*/	
void realloc_table(int new_size);

/*Searched for process with "query" thread_id in p_table and return it*/
process *search(int query);

/*Receives a line, tokenizes it with the " " delimiter and fill an empty process.*/
void line_to_process(process *my_process, char *my_line);

/*Receives a line, gets the parameter t0 and return it*/
float get_t0(char* my_line);

/*Receives a pointer to a process and gives to it a priority*/
int prioritize(process *my_process);

#endif
