//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#ifndef __CICLISTA_H__
#define __CICLISTA_H__

#include <stdlib.h>
#include <unistd.h>
#include "structs.h"
#include "aux.h"
#include "pista.h"
#include "placar.h"

extern pista *velodromo;
extern placar *meu_placar;
extern pthread_barrier_t barreira;
extern pthread_mutex_t mutex_pista;
extern pthread_mutex_t mutex_placar;
extern int relogio;

//inicializa a struct e cria a thread
int cria_ciclista(int posicao, int faixa, int id); 

void destroi_ciclista(ciclista *c);

//Retorna 1 se o ciclista puder pedalar e 0 c.c.
int pode_pedalar(ciclista *c);

//Retorna 1 se o ciclista puder ultrapassar e 0 c.c.
int pode_ultrapassar(ciclista *c);

void reduz_velocidade(ciclista *c);

//Retorna 1 se o ciclista puder voltar para a faixa de origem e 0 c.c.
int consegue_voltar(ciclista *c);

//Retorna o número de vezes que o ciclista deu volta sobre os outros
int volta_outros(ciclista *c);

//Recebe um ponteiro para um ciclista e tenta quebrá-lo. Em êxito, retorna 1 e 0 c.c.
int sorteia_quebra(ciclista *c);

//Recebe um ponteiro para ciclista e sorteia sua velocidade baseado na velocidade da volta anterior
void sorteia_velocidade(ciclista *c);

//Escolhe um ciclista aleatório e sorteia, com 10% de chance, se a sua velocidade passará a ser 90km/h
void sorteia_90();

void *ciclista_generico(void *c);

#endif
