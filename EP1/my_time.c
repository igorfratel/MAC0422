#include "my_time.h"

double time_current() {
	/*Return the time elapsed since its first call*/
	static struct timespec initial; // Time initial
	static int initialized = 0; // Lock to time initial
	
	// Variable
	struct timespec time;

	// Executes only once to start the clock
	if (!initialized) {
		initialized = 1;
		clock_gettime(CLOCK_MONOTONIC_RAW, &initial);
	}
	
	// Gets the time now
	clock_gettime(CLOCK_MONOTONIC_RAW, &time);
	
	return (double)(time.tv_sec - initial.tv_sec) + (double)(time.tv_nsec - initial.tv_nsec)/BILLION; // Time in seconds
}


void my_sleep(float time) {
	/*Sleep implemented time in seconds with one decimal place*/
	static struct timespec nanotime; // Time in a format to function nanosleep

	// Builds the time struct to the nanosleep
	nanotime.tv_sec = (int) time;
	nanotime.tv_nsec = (time - nanotime.tv_sec)*BILLION;

	nanosleep(&nanotime, NULL);
}
