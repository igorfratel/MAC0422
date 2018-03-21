#ifndef MY_TIME_H_
#define MY_TIME_H_

#include <time.h>
#include <stdio.h>
#include <unistd.h>

#define BILLION 1000000000

/*Return the time elapsed since its first call*/
double time_current();

/*Sleep implemented time in seconds with one decimal place*/
void my_sleep(float time);

#endif
