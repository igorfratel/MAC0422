//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#ifndef __PLACAR_H__
#define __PLACAR_H__

#include "structs.h"
#include "aux.h"
#include <stdlib.h>

extern pista *velodromo;
extern placar *meu_placar;

int cria_placar(int voltas, int ciclistas);

void destroi_placar();

int atribui_pontos(ciclista *c, int ordem);

void ordena_placar();

//Retorna a posição do ciclista no ranking
int posicao_ranking(int id);

//Retorna um vetor com as posições do ciclistas de uma determinada volta
nodulo* devolve_volta(int volta);

//Preenche um ciclista id no ranking_geral com determinados pontos em um determinada volta e retorna 1, se a volta está completada.
//0 c.c.
int preenche_volta(ciclista *c);

//Ordena os ciclistas de uma determinada volta em relação aos pontos acumulados
nodulo *ordena_ranking_geral(int volta);

#endif
