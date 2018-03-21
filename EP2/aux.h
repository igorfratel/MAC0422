//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#ifndef __AUX_H__
#define __AUX_H__

#include "structs.h"
#include "placar.h"
#include <stdio.h>
#include <stdlib.h>

extern pista *velodromo;
extern placar *meu_placar;
extern pthread_mutex_t mutex_pista;
extern pthread_mutex_t mutex_placar;
extern int relogio;
extern int debug;

int mod(int a, int b);

void imprime_pista();

void imprime_placar();

//Imprime pontuação final, tempo de chegada e ciclistas quebrados
void imprime_final();

//Atualiza o tempo do relógio
void aumenta_relogio();

int conta_digitos(int a);

//Compara dois ciclistas quanto a quantidade de pontos acumulada
int compara_ciclistas(const void *c1, const void *c2);

//Espera as threads dos ciclistas terminarem
void espera_ciclistas();

int compara_nodulos(const void *nodulo1, const void *nodulo2);

int posicao_ciclista(int id, int volta);

//Imprime as posições dos ciclistas em cada volta. Em volta múltiplo de 10, imprime a pontuação também
void imprime_ciclistas(int volta);

#endif
