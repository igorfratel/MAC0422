//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#ifndef __PISTA_H__
#define __PISTA_H__

#include "aux.h"
#include "structs.h"
#include <stdlib.h>

extern pista *velodromo;
extern placar *meu_placar;

//Recebe um tamanho. Inicializa a pista (matriz de ids de ciclistas global chamada velodromo) com zeros.
int cria_pista(int d, int voltas, int ciclistas);

void destroi_pista();

//muda a posição do ciclista (na pista tbm).
void pedala(ciclista *c);

//realiza ultrapassagem com o ciclista recebido.
void ultrapassa(ciclista *c);

//realiza volta para uma faixa mais próxima da origem (ou a própria) do ciclista recebido
void volta_faixa(ciclista *c);

//determina a ordem de chegada de um ciclista numa determinada volta na linha de chegada
int determina_ordem(ciclista *c);

//busca o ciclista com o respectivo id no vetor de placar e devolte um ponteiro para o ciclista
ciclista *busca_ciclista(int id);

//Retira o ciclista da pista
void finaliza_ciclista(ciclista *c);

#endif
