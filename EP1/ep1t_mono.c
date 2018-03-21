#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "process.h"
#include "process_readers.h"
#include "process_table_handler.h"
#include "generic_process.h"
#include "queue.h"
#include "my_time.h"
#include "aux.h" // mod() and num_context()

/* Global variables */

process_table p_table;
queue *finished_jobs_queue; // Queue where generic_process will push the thread_id of a finished thread
int d = 0; // Error signal

/* Semaphores */

pthread_mutex_t mutex_ptable; // Array p_table
pthread_mutex_t mutex_queue; // Queue of finished jobs

void *shortest_job_first(void *output_file) {
	/*Schedules processes and put the finished ones in the output_file*/
	output_file = (FILE *)output_file;	

	// Main variables
	int index_read; // Process to be read
	int index_thread; // Process to be executed
	int ncpu; // Number of processors
	process_args* args; // Array of args to the processes

	// Auxiliar variables
	int p_cpu; // Process in one cpu;
	int out_line; // Number of the output line of a process
	int realloc_signal; // Signal to know if ocurred a realloc or not
	int first_free_temp; // Number of processes before of a execution
	char output_line[128]; // Line to be printed in the output_file
	process *process_temp; // Pointer to one process in the table

	// Filling variables
	ncpu = sysconf(_SC_NPROCESSORS_ONLN) - 1;

	if (ncpu == 1) args = malloc(sizeof(process_args));
	else {
		args = malloc((ncpu-1)*sizeof(process_args));
		for (int i = 0; i < ncpu-1; i++) args[i].used = 0;
	}
	
	realloc_signal = 0;
	finished_jobs_queue = createQueue();

	p_cpu = 0;
	out_line = 1;
	index_read = 0;
	index_thread = 0;
	
	pthread_mutex_lock(&mutex_ptable); // Lock
	first_free_temp = p_table.first_free;
	pthread_mutex_unlock(&mutex_ptable); // Unlock
	while (1) { // Scheduler
		pthread_mutex_lock(&mutex_ptable); // Lock
		if (p_table.executed_processes < p_table.first_free) {
			// Cuts the table
			if ((float)p_table.executed_processes/p_table.first_free >= 0.5) { // >= 50% of the processes was executed
				index_read = 0;
				realloc_table(1); // Eliminates the executed processes
				realloc_signal = 1;
			}
			// Reorganizes the table
			if (first_free_temp != p_table.first_free) {
				index_read = 0;
				first_free_temp = p_table.first_free;
				if (!realloc_signal) sort_table();
			}

			// Executes the processes
			if (ncpu == 1) { // There is not CPU additional
				// Gets the first process ready to execute
				while (p_table.table[index_read].status != 0) index_read++;

				// Fillings the args
				args[index_thread].t = p_table.table[index_read].dt;
				args[index_thread].trace_name = p_table.table[index_read].trace_name;

				// Starts one process
				if (pthread_create(&p_table.table[index_read].thread_id, NULL, generic_process, &args[index_thread])) {
					printf("ERROR(sfj): Fail to create new process\n");
					exit(1);
				}

				// Process running
				p_table.table[index_read].status = 1;
				
				// Waits for the end of the process
				if (pthread_join(p_table.table[index_read].thread_id, NULL)) {
					printf("ERROR(sfj): Process thread execution failed\n");
					exit(1);
				}
				
				// Process executed
				p_table.table[index_read].status = 3;
				p_table.executed_processes++;
				
				if (d) { // DEBUG
					fprintf(stderr, "DEBUG: The process '%s' has left the system to output line %d\n",
										p_table.table[index_read].trace_name,
										out_line++);
				}

				// Prints processes in a file
				sprintf(output_line, "%s %.1f %.1f %.1f\n", p_table.table[index_read].trace_name, 
												time_current(),
												time_current() - p_table.table[index_read].t0,
												p_table.table[index_read].deadline - time_current());
				fwrite((void*)output_line, 1, strlen(output_line), output_file);

				index_read++;
			}
			else { // There is/are CPU(s) additional
				// Looks for some finished process(es)
				while(!isEmptyQueue(*finished_jobs_queue)) {
					pthread_mutex_lock(&mutex_queue); // Lock
					process_temp = search(popQueue(finished_jobs_queue));
					pthread_mutex_unlock(&mutex_queue); // Unlock
					// Process executed
					process_temp->status = 3;
					p_table.executed_processes++;
					
					if (d) { // DEBUG
						fprintf(stderr, "DEBUG: The process '%s' has left the system to output line %d\n",
											process_temp->trace_name,
											out_line++);
					}

					// Prints processes in a file
					sprintf(output_line, "%s %.1f %.1f %.1f\n", process_temp->trace_name, 
													time_current(),
													time_current() - process_temp->t0,
													process_temp->deadline - time_current());
					fwrite((void*)output_line, 1, strlen(output_line), output_file);

					for (index_thread = 0; index_thread < ncpu - 1; index_thread++)
						if (args[index_thread].unique_id == process_temp->unique_id)
							args[index_thread].used = 0;

					p_cpu--; // One free CPU
				}

				index_thread = 0;
				// Executes the processes
				while (index_read < p_table.first_free && p_cpu < ncpu - 1) {
					if (p_table.table[index_read].status == 0) {
						// Looks for some free arg struct to use
						while (args[index_thread].used == 1) index_thread++;
						// Fillings the args
						args[index_thread].t = p_table.table[index_read].dt;
						args[index_thread].trace_name = p_table.table[index_read].trace_name;
						args[index_thread].used = 1;

						// Starts one process
						if (pthread_create(&p_table.table[index_read].thread_id, NULL, generic_process, &args[index_thread])) {
							printf("ERROR(sfj): Fail to create new process\n");
							exit(1);
						}

						args[index_thread].unique_id = p_table.table[index_read].unique_id;

						// Process running
						p_table.table[index_read].status = 1;

						p_cpu++; // One busy CPU
						index_read++;
						index_thread++; // Next possible process
					}
					else index_read++;
				}
			}
		}
		pthread_mutex_unlock(&mutex_ptable); // Unlock
		my_sleep(0.1);
	}

	return NULL;
}

void *round_robin(void *output_file) {
	/*Schedules processes and put the finished ones in the output_file*/
	output_file = (FILE *)output_file;

	// Main variables
	int index_read; // Process to be read
	int index_thread; // Process to be executed
	int ncpu; // Number of processors
	process_args* args; // Array of args to the processes

	// Auxiliar variables
	int first_free_temp; // Number of processes before of a execution
	int out_line; // Number of the output line of a process
	int processes_pendent; // Number of processes to be executed
	char output_line[128]; // Line to be printed in the output_file
	char trace_name_temp[64]; // Name of the last process to be executed

	// Filling variables
	ncpu = sysconf(_SC_NPROCESSORS_ONLN) - 1;
	strcpy(trace_name_temp, "\0");

	if (ncpu == 1) args = malloc(sizeof(process_args));
	else {
		args = malloc((ncpu-1)*sizeof(process_args));
		for (int i = 0; i < ncpu-1; i++) args[i].used = 0;
	}

	out_line = 1;
	index_thread = 0;
	
	pthread_mutex_lock(&mutex_ptable); // Lock
	first_free_temp = p_table.first_free;
	if (p_table.first_free > 0) index_read = p_table.first_free - 1; 
	else index_read = 0;
	pthread_mutex_unlock(&mutex_ptable); // Unlock
	while (1) {
		pthread_mutex_lock(&mutex_ptable); // Lock
		if (p_table.executed_processes < p_table.first_free) {
			// Cuts the table
			if ((float)p_table.executed_processes/p_table.first_free >= 0.5) // >= 50% of the processes was executed
				realloc_table(1); // Eliminates the executed processes

			// Update the index_read properly, in case of change in the number of processes
			if (first_free_temp != p_table.first_free) {
				if (strcmp(trace_name_temp, "\0")) { // There are processes not finished yet
					index_read = 0;
					while (strcmp(trace_name_temp, p_table.table[index_read].trace_name)) index_read++;
				}
				else index_read = p_table.first_free - 1; // There are just new processes in the table
				first_free_temp = p_table.first_free;
			}

			index_read = (index_read + 1)%p_table.first_free;
			// Executes the processes
			if (ncpu == 1) { // There is not CPU additional
				// Gets the next process to execute
				while (p_table.table[index_read].status != 0 && p_table.table[index_read].status != 2)
					index_read = (index_read + 1)%p_table.first_free;

				// Fillings the args
				args[index_thread].t = QUANTUM;
				args[index_thread].trace_name = p_table.table[index_read].trace_name;

				// Starts one process
				if (pthread_create(&p_table.table[index_read].thread_id, NULL, generic_process, &args[index_thread])) {
					printf("ERROR(rr): Fail to create new process\n");
					exit(1);
				}

				// Process running
				p_table.table[index_read].status = 1;
				
				// Waits for one QUANTUM of the process
				if (pthread_join(p_table.table[index_read].thread_id, NULL)) {
					printf("ERROR(rr): Process thread execution failed\n");
					exit(1);
				}
				
				// Checks if the process has finished or not
				p_table.table[index_read].dt -= QUANTUM;
				if (p_table.table[index_read].dt > 0) { // Process blocked
					p_table.table[index_read].status = 2;
					num_context(1); // Increments the quantity of context switches
				}
				else { // Process executed
					p_table.table[index_read].status = 3;
					p_table.executed_processes++;

					if (d) { // DEBUG
						fprintf(stderr, "DEBUG: The process '%s' has left the system to output line %d\n",
											p_table.table[index_read].trace_name,
											out_line++);
					}
					
					// Prints processes in a file
					sprintf(output_line, "%s %.1f %.1f %.1f\n", p_table.table[index_read].trace_name, 
													time_current(),
													time_current() - p_table.table[index_read].t0,
													p_table.table[index_read].deadline - time_current());
					fwrite((void*)output_line, 1, strlen(output_line), output_file);
				}

				// Load the name of one last process in the table not executed
				if (p_table.executed_processes == p_table.first_free) {
					strcpy(trace_name_temp, "\0");
				}
				else {
					while (p_table.table[index_read].status != 0 && p_table.table[index_read].status != 2) {
						index_read = mod(index_read - 1, p_table.first_free);
					}
					strcpy(trace_name_temp, p_table.table[index_read].trace_name);
				}
			}
			else { // There is/are CPU(s) additional
				// Looks for the finished process(es)
				for (int i = mod(index_read - 1, p_table.first_free); index_thread > 0; i = mod(i - 1, p_table.first_free)) {
					if (p_table.table[i].status == 1) {
						// Waits for one QUANTUM of the process
						if (pthread_join(p_table.table[i].thread_id, NULL)) {
							printf("ERROR(rr): Processs thread execution failed\n");
							exit(1);
						}
						
						// Checks if the process has finished or not
						p_table.table[i].dt -= QUANTUM;
						if (p_table.table[i].dt > 0) { // Process blocked
							p_table.table[i].status = 2;
							num_context(1); // Increments the quantity of context switches
						}
						else { // Process executed
							p_table.table[i].status = 3;
							p_table.executed_processes++;

							if (d) { // DEBUG
								fprintf(stderr, "DEBUG: The process '%s' has left the system to output line %d\n",
													p_table.table[i].trace_name,
													out_line++);
							}
							
							// Prints processes in a file
							sprintf(output_line, "%s %.1f %.1f %.1f\n", p_table.table[i].trace_name, 
															time_current(),
															time_current() - p_table.table[i].t0,
															p_table.table[i].deadline - time_current());
							fwrite((void*)output_line, 1, strlen(output_line), output_file);
						}

						index_thread--; // One free CPU
					}
				}

				// Executes the processes
				if (p_table.executed_processes < p_table.first_free) {
					processes_pendent = p_table.first_free - p_table.executed_processes;
					while(index_thread < ncpu - 1 && processes_pendent > 0) {
						if (p_table.table[index_read].status == 0 || p_table.table[index_read].status == 2) {
							args[index_thread].t = QUANTUM;
							args[index_thread].trace_name = p_table.table[index_read].trace_name;

							// Starts one process
							if (pthread_create(&p_table.table[index_read].thread_id, NULL, generic_process, &args[index_thread])) {
								printf("ERROR(rr): Fail to create new process\n");
								exit(1);
							}

							// Process running
							p_table.table[index_read].status = 1;

							index_thread++; // One busy CPU
							processes_pendent--; // One process was created
							index_read = (index_read + 1)%p_table.first_free;
						}
						else index_read = (index_read + 1)%p_table.first_free;
					}
					// Adjust the index_read to next loop
					index_read = mod(index_read - 1, p_table.first_free);
					// Load the name of one last process in the table not executed
					while (p_table.table[index_read].status == 3) {
						index_read = mod(index_read - 1, p_table.first_free);
					}
					strcpy(trace_name_temp, p_table.table[index_read].trace_name);
				}
				else strcpy(trace_name_temp, "\0");
			}
		}
		pthread_mutex_unlock(&mutex_ptable); // Unlock
		my_sleep(0.1); // Preemption
	}

	return NULL;
}

void *priority_scheduling(void *output_file) {
	/*Schedules processes and put the finished ones in the output_file*/
	output_file = (FILE *)output_file;

	// Main variables
	int index_read; // Process to be read
	int index_thread; // Process to be executed
	int ncpu; // Number of processors
	process_args* args; // Array of args to the processes

	// Auxiliar variables
	int p_cpu; // Process in one cpu;
	int out_line; // Number of the output line of a process
	int first_free_temp; // Number of processes before of a execution
	int processes_pendent; // Number of processes to be executed
	char output_line[128]; // Line to be printed in the output_file
	char trace_name_temp[64]; // Name of the last process to be executed
	process *process_temp; // Pointer to one process in the table
	
	// Filling variables
	ncpu = sysconf(_SC_NPROCESSORS_ONLN) - 1;
	strcpy(trace_name_temp, "\0");

	if (ncpu == 1) args = malloc(sizeof(process_args));
	else {
		args = malloc((ncpu-1)*sizeof(process_args));
		for (int i = 0; i < ncpu-1; i++) args[i].used = 0;
	}

	p_cpu = 0;
	out_line = 1;
	index_thread = 0;
	
	pthread_mutex_lock(&mutex_ptable); // Lock
	first_free_temp = p_table.first_free;
	if (p_table.first_free > 0) index_read = p_table.first_free - 1; 
	else index_read = 0;
	pthread_mutex_unlock(&mutex_ptable); // Unlock
	while (1) {
		pthread_mutex_lock(&mutex_ptable); // Lock
		if (p_table.executed_processes < p_table.first_free) {
			// Cuts the table
			if ((float)p_table.executed_processes/p_table.first_free >= 0.5) // >= 50% of the processes was executed
				realloc_table(1); // Eliminates the executed processes

			// Update the index_read properly, in case of change in the number of processes
			if (first_free_temp != p_table.first_free) {
				if (strcmp(trace_name_temp, "\0")) { // There are processes not finished yet
					index_read = 0;
					while (strcmp(trace_name_temp, p_table.table[index_read].trace_name)) {
						if (index_read != p_table.first_free - 1) index_read++;
						else break;
					}
				}
				else index_read = p_table.first_free - 1; // There are just new processes in the table
				first_free_temp = p_table.first_free;
			}

			index_read = (index_read + 1)%p_table.first_free;
			// Executes the processes
			if (ncpu == 1) { // There is not CPU additional
				// Gets the next process to execute
				while (p_table.table[index_read].status != 0 && p_table.table[index_read].status != 2)
					index_read = (index_read + 1)%p_table.first_free;

				// Prioritize one process
				if (p_table.table[index_read].status == 0)
					p_table.table[index_read].priority = prioritize(&p_table.table[index_read]);

				// Fillings the args
				args[index_thread].t = QUANTUM*p_table.table[index_read].priority;
				if (args[index_thread].t > p_table.table[index_read].dt)
					args[index_thread].t = p_table.table[index_read].dt;

				args[index_thread].trace_name = p_table.table[index_read].trace_name;

				// Starts one process
				if (pthread_create(&p_table.table[index_read].thread_id, NULL, generic_process, &args[index_thread])) {
					printf("ERROR(ps): Fail to create new process\n");
					exit(1);
				}

				// Process running
				p_table.table[index_read].status = 1;
				
				// Waits for some QUANTUMs of the process
				if (pthread_join(p_table.table[index_read].thread_id, NULL)) {
					printf("ERROR(ps): Processs thread execution failed\n");
					exit(1);
				}
				
				// Checks if the process has finished or not
				p_table.table[index_read].dt -= args[index_thread].t;
				if (p_table.table[index_read].dt > 0) { // Process blocked
					p_table.table[index_read].status = 2;
					num_context(1); // Increments the quantity of context switches
				}
				else { // Process executed
					p_table.table[index_read].status = 3;
					p_table.executed_processes++;

					if (d) { // DEBUG
						fprintf(stderr, "DEBUG: The process '%s' has left the system to output line %d\n",
											p_table.table[index_read].trace_name,
											out_line++);
					}
					
					// Prints processes in a file
					sprintf(output_line, "%s %.1f %.1f %.1f\n", p_table.table[index_read].trace_name, 
													time_current(),
													time_current() - p_table.table[index_read].t0,
													p_table.table[index_read].deadline - time_current());
					fwrite((void*)output_line, 1, strlen(output_line), output_file);
				}

				// Load the name of one last process in the table not executed
				if (p_table.executed_processes == p_table.first_free) {
					strcpy(trace_name_temp, "\0");
				}
				else {
					while (p_table.table[index_read].status != 0 && p_table.table[index_read].status != 2) {
						index_read = mod(index_read - 1, p_table.first_free);
					}
					strcpy(trace_name_temp, p_table.table[index_read].trace_name);
				}
			}
			else { // There is/are CPU(s) additional
				// Creates processes
				for (int i = 0; i < p_table.first_free - p_table.executed_processes && index_thread < ncpu - 1; i++) {
					// Gets the first process ready to execute
					while (p_table.table[index_read].status != 0 && p_table.table[index_read].status != 2)
						index_read = (index_read + 1)%p_table.first_free;

					// Prioritize one process
					if (p_table.table[index_read].status == 0)
						p_table.table[index_read].priority = prioritize(&p_table.table[index_read]);

					// Fillings the args
					args[index_thread].t = QUANTUM*p_table.table[index_read].priority;
					if (args[index_thread].t > p_table.table[index_read].dt)
						args[index_thread].t = p_table.table[index_read].dt;
					args[index_thread].trace_name = p_table.table[index_read].trace_name;

					// Starts one process
					if (pthread_create(&p_table.table[index_read].thread_id, NULL, generic_process, &args[index_thread])) {
						printf("ERROR(ps): Fail to create new process\n");
						exit(1);
					}

					// Process running
					p_table.table[index_read].status = 1;

					index_thread++;
					index_read = (index_read + 1)%p_table.first_free;
				}

				// Waits for the processes
				for (int i = mod(index_read - 1, p_table.first_free); index_thread > 0; i = mod(i - 1, p_table.first_free)) {
					// Gets the first process running
					if (p_table.table[i].status == 1) {
						// Waits for the end of the process
						if (pthread_join(p_table.table[i].thread_id, NULL)) {
							printf("ERROR(ps): Process thread execution failed\n");
							exit(1);
						}

						// Checks if the process has finished or not
						p_table.table[i].dt -= QUANTUM*p_table.table[i].priority;
						if (p_table.table[i].dt > 0) { // Process blocked
							p_table.table[i].status = 2;
							num_context(1); // Increments the quantity of context switches
						}
						else { // Process executed
							p_table.table[i].status = 3;
							p_table.executed_processes++;

							if (d) { // DEBUG
								fprintf(stderr, "DEBUG: The process '%s' has left the system to output line %d\n",
													p_table.table[i].trace_name,
													out_line++);
							}
							
							// Prints processes in a file
							sprintf(output_line, "%s %.1f %.1f %.1f\n", p_table.table[i].trace_name, 
															time_current(),
															time_current() - p_table.table[i].t0,
															p_table.table[i].deadline - time_current());
							fwrite((void*)output_line, 1, strlen(output_line), output_file);
						}

						index_thread--;
					}
				}

				index_read = mod(index_read - 1, p_table.first_free);

				if (p_table.executed_processes == p_table.first_free) {
					strcpy(trace_name_temp, "\0");
				}
				else {
					while (p_table.table[index_read].status == 3) {
						index_read = mod(index_read - 1, p_table.first_free);
					}
					strcpy(trace_name_temp, p_table.table[index_read].trace_name);
				}
			}
		}
		pthread_mutex_unlock(&mutex_ptable); // Unlock
		my_sleep(0.1); // Preemption
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	/*Simulation of process scheduler*/

	// argv[1] = scheduler
	// argv[2] = trace_path
	// argv[3] = output_path
	// argv[4] = d (optional)

	// Arguments variables
	FILE *trace_file;
	FILE *output_file;

	// Thread argument
	pthread_t tid_ps;
	
	// Auxiliar argument
	char context_switches[16];

	// Error signal
	if (argc == 5) {
		if (!strcmp(argv[4], "d")) d = 1;
		else {
			printf("ERROR(main): Fourth invalid argument\n");
			exit(1);
		}
	}

	// Open files
	if ((trace_file = fopen(argv[2], "r")) == NULL || (output_file = fopen(argv[3], "w")) == NULL) {
		printf("ERROR(main): Invalid file\n");
		exit(1);
	}

	//Allocates the process table
	if (emalloc_table()) {
		printf("ERROR(main): failed to dinamically allocate the process table\n");
		exit(1);
	}

	// Initializes semaphores
	pthread_mutex_init(&mutex_ptable, NULL);
	pthread_mutex_init(&mutex_queue, NULL);

	// Initializes queue
	finished_jobs_queue = NULL;

	// Chooses the scheduler
	if (!strcmp(argv[1], "1")) {
		if (pthread_create(&tid_ps, NULL, shortest_job_first, (void*)output_file)) {
			printf("ERROR(main): Fail to create the scheduler SJB\n");
			exit(1);
		}
	}
	else if (!strcmp(argv[1], "2")) {
		if (pthread_create(&tid_ps, NULL, round_robin, (void*)output_file)) {
			printf("ERROR(main): Fail to create the scheduler RR\n");
			exit(1);
		}
	}
	else if (!strcmp(argv[1], "3")) {
		if (pthread_create(&tid_ps, NULL, priority_scheduling, (void*)output_file)) {
			printf("ERROR(main): Fail to create the scheduler PS\n");
			exit(1);
		}
	}
	else printf("ERROR(main): Invalid first argument\n");

	// Calls the process reader
	process_reader(trace_file);

	// Waits for the end of the process scheduler
	while (p_table.first_free != p_table.executed_processes) continue;
	if (pthread_cancel(tid_ps)) {
		printf("ERROR(main): Scheduler thread execution failed\n");	
		exit(1);
	}
	
	//Puts the context switches in the output file
	sprintf(context_switches, "%d", num_context(0));
	fwrite((void*)context_switches, 1, strlen(context_switches), output_file);
	
	// Close files
	fclose(trace_file);
	fclose(output_file);
	// Destroyes
	pthread_mutex_destroy(&mutex_ptable);
	pthread_mutex_destroy(&mutex_queue);
	if (finished_jobs_queue != NULL)
		destroyQueue(finished_jobs_queue);
	// Free memory
	for (int i = 0; i < p_table.size; i++) free(p_table.table[i].trace_name);
	free(p_table.table);
	
	return 0;
}
