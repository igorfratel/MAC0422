#ifndef AUX_H_
#define AUX_H_

#include <stdio.h>

extern int d; // Error signal

/*Returns a%b*/
int mod(int a, int b);

/*Increment (or not) quantity_context and return it*/
int num_context(int increment);

#endif