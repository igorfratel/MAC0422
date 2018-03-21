# compiler
  CC = gcc

# compiler flags:
  #-lreadline references the GNU readline library
  #-Wall turns on most, but not all, compiler warnings
  CFLAGS  = -lreadline -pthread

# the build target executable:

default: ep1sh ep1

ep1sh: ep1sh.c
	$(CC) ep1sh.c $(CFLAGS) -o ep1sh

ep1: ep1.c my_time.o process_readers.o process_table_handler.o generic_process.o queue.o aux.o
	$(CC) ep1.c $(CFLAGS) -o ep1 process_readers.o process_table_handler.o generic_process.o my_time.o queue.o aux.o

my_time: my_time.c my_time.h
	(CC) $(CFLAGS) -c my_time

process_readers: process_readers.c process_readers.h
	(CC) $(CFLAGS) -c process_readers

process_table_handler: process_table_handler.c process_table_handler.h
	(CC) $(CFLAGS) -c process_table_handler

generic_process: generic_process.c generic_process.h
	(CC) $(CFLAGS) -c generic_process

queue: queue.c queue.h
	(CC) $(CFLAGS) -c queue

aux: aux.c aux.h
	(CC) $(CFLAGS) -c aux

clean:
	$(RM) ep1 ep1sh *.o *~
