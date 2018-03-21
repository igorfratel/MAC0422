#include "process_table_handler.h"

int compare_dt(const void *a, const void *b) {
	/* 0 == ready; 1 == running; 2 == blocked; 3 == finished
	/*Orders first by status in descending order. If status is equal, orders by dt in ascending order*/
	if ((*(process*)a).status > (*(process*)b).status) return -1;
    if ((*(process*)a).status < (*(process*)b).status) return 1 ;
    if ((*(process*)a).dt == (*(process*)b).dt)
    	return (int)((*(process*)a).deadline - (*(process*)b).deadline);
    return ((*(process*)a).dt - (*(process*)b).dt);
    	
}

void sort_table() {
	/*Sorts the extern global variable process_table in ascending order*/
	qsort(p_table.table, p_table.first_free, sizeof(process), compare_dt);
}

process *search(int query) {
	for (int i = 0; i < p_table.first_free; i++) {
		if (query == p_table.table[i].unique_id){
			return &p_table.table[i];
		}
	}
}


int emalloc_table(){
	/*Allocates the process_table with one process. Returns 1 in case of error and 0 otherwise*/
	if((p_table.table = malloc(sizeof(process))) == NULL) return 1;
	p_table.size = 1;
	p_table.first_free = 0;
	p_table.executed_processes = 0;
	p_table.table[0].trace_name = NULL;
	return 0;
}

void realloc_table(int new_size) {
	/*Creates a new table with new_size * p_table.size * sizeof(process)
	 *and assigns the process_table pointers to this new table.
	 *Removes all executed processes*/

	// Variables
	process *new_table;
	int i; // Index to the new_table
	int j; // Index to the table

	// Filling variables
	i = 0;
	j = 0;
	new_table = malloc(new_size*p_table.size*sizeof(process));
	for(int k = 0; k < new_size*p_table.size; k++) new_table[k].trace_name = NULL;

	while (j < p_table.first_free) {
		if (p_table.table[j].status != 3) {
			new_table[i] = p_table.table[j];
			i++;
		}
		else // Status == 3 -> one executed process
			free(p_table.table[j].trace_name);
		j++;
	}

	free(p_table.table);
	p_table.table = new_table;
	p_table.size = new_size*p_table.size;
	p_table.executed_processes = 0;
	p_table.first_free = i;
}

void line_to_process(process *my_process, char *my_line) {
	/*Receives a line, tokenizes it with the " " delimiter and fill an empty process.*/
	static int unique_id;
	
	// Variables
	int caracteres[4]; // Number of caracteres of each one token in my_line
	int c; // Auxiliar variable to number of caracteres
	int d;
	int c0; // Auxiliar variable
	char *t0; // String of t0
	char *dt; // String of dt
	char *deadline; // String of deadline

	// Computes the number of digits of t0
	for (c = 0; my_line[c] != 32; c++);
	caracteres[0] = c;
	// Computes the number of digits of dt
	d = c+1; // Begin of the string dt
	for (c = d; my_line[c] != 32; c++);
	caracteres[1] = c - d;
	// Computes the number of digits of deadline
	d = c+1; // Begin of the string deadline
	for (c = d; my_line[c] != 32; c++);
	caracteres[2] = c - d;
	// Computes the number of caracteres of the name of the process
	d = c+1; // Begin of the string trace_name
	for (c = d; my_line[c] != 0; c++);
	caracteres[3] = c - d;

	// Creates the empty strings
	t0 = malloc(caracteres[0]+1);
	dt = malloc(caracteres[1]+1);
	deadline = malloc(caracteres[2]+1);
	my_process->trace_name = malloc(caracteres[3]+1);
	
	// Fills the strings
	c = caracteres[0];
	for (int i = 0; i < c; i++) t0[i] = my_line[i]; // Gets the number t0 from my_line
	t0[c] = 0;

	c0 = c;
	c = caracteres[1];
	for (int i = c0+1, j = 0; j < c; i++, j++) dt[j] = my_line[i]; // Gets the number dt from my_line
	dt[c] = 0;
	
	c0 = c0+c+1;
	c = caracteres[2];
	for (int i = c0+1, j = 0; j < c; i++, j++) deadline[j] = my_line[i]; // Gets the number deadline from my_line
	deadline[c] = 0;

	c0 = c0+c+1;
	c = caracteres[3];
	for (int i = c0+1, j = 0; j < c; i++, j++) my_process->trace_name[j] = my_line[i]; // Gets the string trace_name from my_line
    if(my_process->trace_name[c - 1] == '\n')
		my_process->trace_name[c - 1] = 0;
	else
		my_process->trace_name[c] = 0;

	// Fills the empty process
	my_process->status = 0;
	my_process->t0 = atof(t0);
	my_process->dt = atof(dt);
	my_process->deadline = atof(deadline);
	my_process->unique_id = unique_id++;

	free(t0);
	free(dt);
	free(deadline);
}

float get_t0(char* my_line) {
	/*Receives a line, gets the parameter t0 and return it*/

	// Variables
	int digit; // Number of digits
	char *t0_s; // String of t0
	float t0; // Number t0

	// Computes the number of digits of t0
	for (digit = 0; my_line[digit] != 32; digit++);

	// Create the empty string
	t0_s = malloc(digit+1);

	// Fills the string
	for (int i = 0; i < digit; i++) t0_s[i] = my_line[i]; // Gets the number t0 from my_line
	t0_s[digit] = 0; // End of the string

	// Gets the number t0
	t0 = atof(t0_s);

	free(t0_s);
	return t0;
}

int prioritize(process *my_process) {
	/*Receives a pointer to a process and gives to it a priority*/
	int surplus = my_process->deadline - (my_process->dt + my_process->t0); // available time beyond the minimum required
	if (surplus <= 2)
		return 10; // 1 segundo

	if (surplus <= 4)
		return 8;

	if (surplus <= 6)
		return 6;

	if (surplus <= 8)
		return 4;
		
	return 2;
}
