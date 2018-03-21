#ifndef PROCESS_READERS_H_
#define PROCESS_READERS_H_

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "my_time.h"
#include "process.h"
#include "process_table_handler.h"

extern int d;
extern process_table p_table;
extern pthread_mutex_t mutex_ptable;

/*Receives a trace file and fills the process_table global variable as the processes "arrive"*/
void process_reader(FILE *trace_file);

#endif
