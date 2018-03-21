#include "process_readers.h"

void process_reader(FILE *trace_file) {
	/*Receives a trace file and fills the process_table global variable as the processes "arrive"*/
	int trace_line = 1; // Number of the trace line of a process
	char my_line[128];
	float arrival_time;
	time_current(); // Starts running the clock
	while(fgets(my_line, 128, trace_file) != NULL) {
		pthread_mutex_lock(&mutex_ptable); // Lock
		if (p_table.first_free >= p_table.size)
			realloc_table(2);
		pthread_mutex_unlock(&mutex_ptable); // Unlock

		arrival_time = get_t0(my_line);
		while(time_current() < arrival_time) { // Waits for the time of the process
			my_sleep(0.1);
		}

		pthread_mutex_lock(&mutex_ptable); // Lock
		line_to_process(&(p_table.table[p_table.first_free]), my_line);
		if (d) { // DEBUG
			fprintf(stderr, "DEBUG: The process '%s' from trace line %d arrived in the system\n",
								p_table.table[p_table.first_free].trace_name,
												trace_line++);
		}
		p_table.first_free++; // Next process
		pthread_mutex_unlock(&mutex_ptable); // Unlock
	}
}
