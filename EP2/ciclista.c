//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#include "ciclista.h"

int cria_ciclista(int posicao, int faixa, int id) {
	ciclista *c = malloc(sizeof(ciclista));
	if (c == NULL) return 0;
	c->v = 30;
	c->volta = 0; //Pois o ciclista não iniciou uma volta (ainda)
	c->pos = posicao;
	c->faixa = faixa;
	c->faixa_origem = faixa;
	c->avaliado = 0;
	c->v_90 = 0;
	c->pontos = 0;
	c->quebrado = 0;
	c->ultima_outros = 0;
	c->termino_corrida = 0;
	c->id = id;
	pthread_mutex_lock(&mutex_pista);
	velodromo->circuito[faixa][posicao] = id;
	pthread_mutex_unlock(&mutex_pista);
	meu_placar->ranking[id - 1] = c;
	pthread_create(&(c->thread_id), NULL, ciclista_generico, c);
	return 1;
}

void destroi_ciclista(ciclista *c) {
	free(c);
}

int pode_pedalar(ciclista *c) {
	switch (c->v) {
		case 30:
			if (relogio%120 == 0)
				return 1;
			break;
		case 60:
			if (relogio%60 == 0)
				return 1;
			break;
		case 90:
			if (relogio%40 == 0)
				return 1;
	}

	return 0;
}

int pode_ultrapassar(ciclista *c) {
	ciclista *c_frente;
	if (c->faixa == 9 || velodromo->circuito[c->faixa + 1][mod(c->pos + 1, velodromo->tamanho)]) return 0;
	c_frente = busca_ciclista(velodromo->circuito[c->faixa][mod(c->pos + 1, velodromo->tamanho)]);
	if (c->v <= c_frente->v) return 0;

	return 1;
}

void reduz_velocidade(ciclista *c) {
	ciclista *c_frente;
	c_frente = busca_ciclista(velodromo->circuito[c->faixa][mod(c->pos + 1, velodromo->tamanho)]);
	if (c->v > c_frente->v) c->v = c_frente->v;
}

int consegue_voltar(ciclista *c) {
	if (velodromo->circuito[c->faixa - 1][mod(c->pos + 1, velodromo->tamanho)]) return 0;
	return 1;
}

int volta_outros(ciclista *c) {
	int distancia, qtde_voltas = 0;
	if (c->volta < velodromo->maior_volta || c->volta < c->ultima_outros + 2) return 0;
	if (c->volta - velodromo->maior2_volta < 2) return 0;
	for (distancia = c->volta - velodromo->maior2_volta; distancia >= 2; distancia -= 2)
		qtde_voltas++;
	c->ultima_outros = c->volta;
	return qtde_voltas;
}

int sorteia_quebra(ciclista *c) {
	int rand_q, posicao; //posicao no ranking
	rand_q = rand()%100 + 1; //Gera número aleatório entre 1 e 100

	if (rand_q == 1) {
		posicao = posicao_ranking(c->id);
		c->quebrado = c->volta;
		printf("Ciclista quebrado\n\nID: %d\nRanking: %d\nVolta em que quebrou: %d\n\n", c->id, posicao, c->quebrado);
		finaliza_ciclista(c);
		velodromo->quebrados++;
		return 1;
	}

	return 0;
}

void sorteia_velocidade(ciclista *c) {
	int rand_v;

	rand_v = rand()%10 + 1; //Gera número aleatório entre 1 e 10

	switch (c->v) { 
		case 30:
			//70% de chance de escolher 60
			if (rand_v <= 7) c->v = 60;
			//30% de chance de escolher 30
			else c->v = 30;
			break;
		case 60:
			//50% de chance de escolher 60 ou 30
			if (rand_v <= 5) c->v = 60;
			else c->v = 30;
	}
	if (c->v > velodromo->v_max)
		velodromo->v_max = c->v;
}

void sorteia_90() {
	int rand_c, rand_v;
	static int flag = 0;
	if (flag) return;
	
	rand_c = rand()%velodromo->total_ciclistas; //ciclista aleatório
	rand_v = rand()%10 + 1; //sorteio de velocidade

	//Se quebrou ou já terminou a corrida, pega o próximo
	while (meu_placar->ranking[rand_c]->volta == velodromo->numero_voltas + 1)
		rand_c = (rand_c + 1)%velodromo->total_ciclistas;

	if (rand_v == 1)
		meu_placar->ranking[rand_c]->v_90 = 1;
	flag = 1;
}

#define cast_c ((ciclista *) c)

void *ciclista_generico(void *c) {
	int id_primeiro, pedalei = 0, quebrou = 0;
	ciclista *c_primeiro;
	while (cast_c->volta <= velodromo->numero_voltas || velodromo->finalizados < velodromo->total_ciclistas) {
		cast_c->avaliado = 0;
		pthread_barrier_wait(&barreira);

		//Espera ciclistas a frente tomarem suas decisões
		if ((id_primeiro = velodromo->circuito[cast_c->faixa][mod(cast_c->pos + 1, velodromo->tamanho)])) {
			pthread_mutex_lock(&mutex_placar);
			c_primeiro = busca_ciclista(id_primeiro);
			pthread_mutex_unlock(&mutex_placar);
			while (!c_primeiro->avaliado) sleep(0);
		}

		if (cast_c->faixa != cast_c->faixa_origem && (id_primeiro = velodromo->circuito[cast_c->faixa - 1][mod(cast_c->pos + 1, velodromo->tamanho)])) {
			pthread_mutex_lock(&mutex_placar);
			c_primeiro = busca_ciclista(id_primeiro);
			pthread_mutex_unlock(&mutex_placar);
			while (!c_primeiro->avaliado) sleep(0);
		}

		if (cast_c->faixa != 9 && (id_primeiro = velodromo->circuito[cast_c->faixa + 1][mod(cast_c->pos + 1, velodromo->tamanho)])) {
			pthread_mutex_lock(&mutex_placar);
			c_primeiro = busca_ciclista(id_primeiro);
			pthread_mutex_unlock(&mutex_placar);
			while (!c_primeiro->avaliado) sleep(0);
		}
		
		pthread_mutex_lock(&mutex_pista);
		if (pode_pedalar(cast_c)) {
			if (cast_c->faixa_origem != cast_c->faixa && consegue_voltar(cast_c)) {
				volta_faixa(cast_c); //Se aproxima da faixa original
				pedalei = 1;
			}
			else if (velodromo->circuito[cast_c->faixa][mod(cast_c->pos + 1, velodromo->tamanho)]) {
				if (pode_ultrapassar(cast_c)) {
					ultrapassa(cast_c);
					pedalei = 1;
				}
				else reduz_velocidade(cast_c);
			}
			else {
				pedala(cast_c);
				pedalei = 1;
			}
		}
		pthread_mutex_unlock(&mutex_pista);

		if (pedalei) {
			if (!cast_c->pos) { // Linha de chegada
				cast_c->volta++;

				if (cast_c->volta > velodromo->maior_volta)
					velodromo->maior_volta = cast_c->volta;
				else if (cast_c->volta > velodromo->maior2_volta)
					velodromo->maior2_volta = cast_c->volta;

				//Pontuação: volta sobre todos os outros
				cast_c->pontos += volta_outros(cast_c)*20;

				if (cast_c->volta > 1) { //Sprint
					pthread_mutex_lock(&mutex_placar);
					if (preenche_volta(cast_c)) imprime_ciclistas(cast_c->volta);
					pthread_mutex_unlock(&mutex_placar);
				}
				if (cast_c->volta > velodromo->numero_voltas) { //Término de corrida
					pthread_mutex_lock(&mutex_pista);
					finaliza_ciclista(cast_c);
					pthread_mutex_unlock(&mutex_pista);
				}
				else if (cast_c->volta > 1)	{
					pthread_mutex_lock(&mutex_pista);
					if (cast_c->volta%15 == 0 && velodromo->total_ciclistas - velodromo->finalizados > 5) {
						quebrou = sorteia_quebra(cast_c);
					}
					if (!quebrou) {
						if (velodromo->numero_voltas - cast_c->volta == 1) {
							sorteia_90();
							if (cast_c->v_90) cast_c->v = 90;
						}
						sorteia_velocidade(cast_c);
					}
					pthread_mutex_unlock(&mutex_pista);
					quebrou = 0;
				}
			}
			pedalei = 0;
		}

		cast_c->avaliado = 1;
		if (pthread_barrier_wait(&barreira) == PTHREAD_BARRIER_SERIAL_THREAD) {
			aumenta_relogio();
		}
	}

	return NULL;
}
