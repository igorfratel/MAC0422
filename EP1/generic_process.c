#include "generic_process.h"

void *generic_process(void *my_process) {
	/*Receives a process and execute it for a determined time*/
	int cpu_num; // Number of the CPU

	if (d) { // DEBUG
		if ((cpu_num = sched_getcpu()) < 0) {
			printf("ERROR(%s)): Fail to get the CPU in use\n", ((process_args *)my_process)->trace_name);
			exit(1);
		}

		fprintf(stderr, "DEBUG: The process '%s' is using CPU %d\n", ((process_args *)my_process)->trace_name, cpu_num);
	}
	
	// Execution of process
	my_sleep(((process_args *)my_process)->t);
	
	if (d) { // DEBUG
		if ((cpu_num = sched_getcpu()) < 0) {
			printf("ERROR(%s)): Fail to get the CPU in use\n", ((process_args *)my_process)->trace_name);
			exit(1);
		}

		fprintf(stderr, "DEBUG: The process '%s' is leaving CPU %d\n", ((process_args *)my_process)->trace_name, cpu_num);
	}
	
	// End of the process
	if (finished_jobs_queue != NULL) {
		pthread_mutex_lock(&mutex_queue); // Lock
		pushQueue(finished_jobs_queue, ((process_args *)my_process)->unique_id);
		pthread_mutex_unlock(&mutex_queue); // Unlock
	}
	
	return NULL;
}
