//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <pthread.h>
//Conterá a struct ciclista e os métodos dos ciclistas
typedef struct {
	int v; //velocidade
	int volta;
	int pos; //índice da pista no qual o ciclista está	
	int faixa; //em qual faixa o ciclista está
	int faixa_origem;
	int avaliado; //1 se já tomou sua decisão na pista, 0 c.c.
	int v_90; //Sorteado para a velocidade de 90km/h
	int pontos;
	int quebrado; //Número da volta em que o ciclista quebrou, 0 caso não tenha quebrado
	int ultima_outros; //Última volta em que o ciclista deu uma volta sobre todos
	int termino_corrida; //Instante de tempo em que o ciclista cruzou a linha de chegada no final da corrida
	int id; //Deve ser diferente de 0
	pthread_t thread_id;
} ciclista;

//Conterá a struct pista e os métodos da pista
typedef struct {
	int **circuito;
	int total_ciclistas;
	int finalizados; //Ciclistas que terminaram a corrida
	int quebrados; //Ciclistas que quebraram durante a corrida
	int numero_voltas;
	int tamanho;
	int v_max; //velocidade máxima corrente na pista
	int maior_volta, maior2_volta; //A primeira e segunda maior quantidade de volta dada entre os ciclistas
} pista;

typedef struct {
	int id;
	int pontos_acumulado;
} nodulo;

typedef struct {
	ciclista **ranking; //Situação dos pontos dos ciclistas no momento atual
	nodulo **ranking_geral; //Situação dos pontos e posições de cada ciclista em cada volta
} placar;

#endif
