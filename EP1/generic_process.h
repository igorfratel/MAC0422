#ifndef GENERIC_PROCESS_H_
#define GENERIC_PROCESS_H_

#define _GNU_SOURCE  //Required by sched_getcpu()
#include <pthread.h>
#include <stdlib.h>
#include <sched.h> // CPU info
#include "process.h"
#include "my_time.h"
#include "queue.h"

extern int d; // Error signal
extern queue *finished_jobs_queue; // Queue where generic_process will push the thread_id of a finished thread
extern pthread_mutex_t mutex_queue; // Queue of finished jobs

/*Process args strucutre*/
typedef struct
{
	int used; // 0 == not; 1 == yes;
	float t; // Time to execute the process
	char *trace_name; // Name of the process
	int unique_id; // Thread of the process itself
} process_args;

/*Receives a process and execute it for a determined time*/
void *generic_process(void *my_process);

#endif
