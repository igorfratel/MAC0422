#include "aux.h"

int mod(int a, int b) {
	/*Returns a%b*/
	if(b < 0)
		return mod(a, -b);
	int ret = a % b;
	if(ret < 0)
		ret+=b;
	return ret;
}

int num_context(int increment) {
	/*Increment (or not) quantity_context and return it*/
	static int quantity_context = 0;

	if (increment) {
		quantity_context++;
		if (d) // DEBUG
			fprintf(stderr, "DEBUG: Context switches until now: %d\n", quantity_context);
	}

	return quantity_context;
}