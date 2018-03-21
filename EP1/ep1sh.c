#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*In the C and C++ programming languages, unistd.h is the name of the header file that provides 
 *access to the POSIX operating system API.*/
#include <unistd.h> 
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h> //pid_t 
#include <sys/wait.h> //waitpid()
#include <time.h>
#include <grp.h>


void builtin_date() {
	/*Displays the current date and time in the linux date command format.*/
    time_t t;
   	struct tm *info;
   	char buffer[64];

   	t = time(NULL);
   	info = localtime(&t);
   	strftime(buffer, 80, "%a %b %d %X %Z %Y", info);
   	printf("%s\n", buffer);
}

void builtin_chown(char **args) {
	/*Receives ":group" and "file" arguments. Executes Unix chown*/
	struct group *grp;
	gid_t  gid;

	args[1]++; //remove ":" at the beggining of <group> agr.
	grp = getgrnam(args[1]);
	if (grp == NULL) 
		printf("builtin_chown: Failed to get group id\n");
	else {
  		gid = grp->gr_gid;
		chown(args[2], -1, gid);
	}
}

char **my_realloc(char **matrix, int old_size, int new_size) {
	/*Receives a char matrix and a new size. Creates a new matrix with size new_size, copies all the data from the old
	 *matrix to the new one and frees the old matrix.*/
	char **new_matrix;

	new_matrix = malloc(new_size*sizeof(char*));
	for (int i = 0; i < old_size; i++) {
		new_matrix[i] = matrix[i];
	}
	free(matrix);
	return new_matrix;
}

char *prompt_generator() {
	/*Returns the specified prompt.*/
	char *prompt; //will contain the complete prompt.
	size_t size_dirpath; //will contain the max size of a directory path in this system.
	char *buffer_dirpath; //will contain the current directory path.

	size_dirpath = pathconf(".", _PC_PATH_MAX); //Reads from the sys configs the max possible size for a directory path.
	buffer_dirpath = malloc(size_dirpath); //allocates for the buffer the maximum dirpath size.
    getcwd(buffer_dirpath, size_dirpath); //get the current directory path and puts it in buffer_dirpath.
	prompt = malloc(size_dirpath + 4);
	snprintf(prompt, size_dirpath + 4, "[%s]$ ", buffer_dirpath);
	free (buffer_dirpath);
    return prompt;
}

char *shell_read (char *prompt) {
 	/*Prints the prompt, reads a line from the stdin and returns it*/
 	return readline(prompt);	
  }

char **shell_parse (char *line) {
	/*Receives a line, tokenizes it with the " " delimiter and returns a char matrix where each line is a token.*/
	int n; //number of tokens
	char s[2]; //delimiter
	int imalloc; //auxiliar variable
	char *token;
	char **tokens;

	strcpy(s, " ");
	n = 0;
	imalloc = 2;
	tokens = malloc(imalloc*sizeof(char*));
	token = strtok(line, s);
	while (token != NULL) {
		if (n >= imalloc) {
			tokens = my_realloc(tokens, imalloc, imalloc*2);
			imalloc *= 2;
		}
		tokens[n++] = token;
		token = strtok(NULL, s);
	}
	if (n >= imalloc)
		tokens = my_realloc(tokens, imalloc, imalloc + 1); 
	tokens[n] = NULL;
	return tokens;
}

int shell_execute (char **args) {
	/*Look for built-in commands or bin/ commands...*/
	pid_t pid;

	if (strcmp(args[0], "date") == 0)
		builtin_date();
	else if (strcmp(args[0], "chown") == 0)
		builtin_chown(args);
	else {
		pid = fork();
		if (pid != 0) {
			/*Father process code*/
			waitpid(pid, NULL, WUNTRACED); //The null argument should be discussed. WUNTRACED returns if a child process has terminated.
		}
		else {
			/*child process code*/
			/*execv params: int execv(const char *path, char *const argv[]);*/
			/*manpage: The execv(), execvp(), and execvpe()  functions  provide  an  array  of
	       	 *pointers  to  null-terminated  strings that represent the argument list
	         *available to the new  program.   The  first  argument,  by  convention,
	         *should  point  to the filename associated with the file being executed.
	         *The array of pointers must be terminated by a null pointer.
			*/
			execv(args[0], args);
		}
	}
	return 0;
}

void shell_loop() {
	/*Read, Parse, Execute*/
	char *line; //This will contain the line read.
	char **args; //This will contain the parsed line, after shell_parse(line) is called.
	char *prompt;

	prompt = prompt_generator();
	line = NULL;
	args = NULL;
	system("clear");
	while(1) {
		if (line != NULL)
			free(line);	
		if (args != NULL)
			free(args);
		line = shell_read(prompt);
		if (strcmp(line, "exit") == 0) {
			free(prompt);
			free(line);
			clear_history();
			exit(1);
		}
		if (line != NULL) {
			add_history(line);        
			args = shell_parse(line);
			if (args[0] != NULL)
				shell_execute(args); 
		}
	}
}

int main(int argc, char **argv) {
	shell_loop();
}	
