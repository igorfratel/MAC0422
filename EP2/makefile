#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

# compiler
  CC = gcc

# compiler flags:
  CFLAGS  = -pthread -Wall

# the build target executable:

default: ep2

ep2: ep2.c ciclista.o pista.o placar.o aux.o
	$(CC) ep2.c $(CFLAGS) -o ep2 ciclista.o pista.o placar.o aux.o

pista: pista.c pista.h
	(CC) $(CFLAGS) -c pista

placar: placar.c placar.h
	(CC) $(CFLAGS) -c placar

ciclista: ciclista.c ciclista.h
	(CC) $(CFLAGS) -c ciclista

aux: aux.c aux.h
	(CC) $(CFLAGS) -c aux

clean:
	$(RM) ep1 ep1sh *.o *~
