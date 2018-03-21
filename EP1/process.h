#ifndef PROCESS_H_
#define PROCESS_H_

#include <pthread.h>

/*Process structure*/
typedef struct
{
	int status; // 0 == ready; 1 == running; 2 == blocked; 3 == finished
	int priority; // Priority of the process
	float t0; // Time of arrival
	float dt; // Required processing time
	float deadline; // Deadline for execution
	char* trace_name; // Name of the process as specified in the trace_file
	pthread_t thread_id; // Thread of the process itself
	int unique_id;
} process;

typedef struct
{
	process *table;
	int size;
	int first_free;
	int executed_processes;
} process_table;

#endif
