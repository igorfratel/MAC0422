//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#include "placar.h"

int cria_placar(int voltas, int ciclistas) {
	int i, j;
	ciclista **ranking = (ciclista**)malloc(ciclistas*sizeof(ciclista*));
	nodulo **ranking_geral = (nodulo**)malloc(voltas*sizeof(nodulo*));
	if (ranking == NULL || ranking_geral == NULL) return 0;
	for (i = 0; i < ciclistas; i++) {
		ranking[i] = NULL;
	}
	for (i = 0; i < voltas; i++) {
		ranking_geral[i] = (nodulo*)malloc(ciclistas*sizeof(nodulo));
	}
	for (i = 0; i < voltas; i++) 
		for (j = 0; j < ciclistas; j++) {
			ranking_geral[i][j].id = -1;
			ranking_geral[i][j].pontos_acumulado = -1;
		}
	meu_placar->ranking = ranking;
	meu_placar->ranking_geral = ranking_geral;
	return 1;
}

void destroi_placar() {
	int i;
	free(meu_placar->ranking);
	for (i = 0; i < velodromo->numero_voltas; i++)
		free(meu_placar->ranking_geral[i]);
	free(meu_placar->ranking_geral);
	free(meu_placar);
}

int atribui_pontos(ciclista *c, int ordem) {
	switch(ordem) {
		case 1:
			c->pontos += 5;
			return 5;
		case 2:
			c->pontos += 3;
			return 3;
		case 3:
			c->pontos += 2;
			return 2;
		case 4:
			c->pontos += 1;
			return 1;
	}
	
	return 0;
}

void ordena_placar() {
	qsort(meu_placar->ranking, velodromo->total_ciclistas, sizeof(ciclista*), compara_ciclistas);
}

int posicao_ranking(int id) {
	int i;
	ordena_placar();
	for (i = 0; i < velodromo->total_ciclistas; i++) {
		if (meu_placar->ranking[i]->id == id) break;
	}

	return i + 1;
}

int preenche_volta(ciclista *c) {
	int i;
	for (i = 0; i < velodromo->total_ciclistas; i++)
		if (meu_placar->ranking_geral[c->volta-2][i].id == -1) {
			meu_placar->ranking_geral[c->volta-2][i].id = c->id;
			meu_placar->ranking_geral[c->volta-2][i].pontos_acumulado = c->pontos;
			if (c->volta%10 == 1)
				meu_placar->ranking_geral[c->volta-2][i].pontos_acumulado += atribui_pontos(c, i + 1);
			break;
		}
	
	if (i + 1 + velodromo->quebrados == velodromo->total_ciclistas) return 1;
	return 0;
}

nodulo *ordena_ranking_geral(int volta) {
	int i;
	nodulo *ranking_volta = (nodulo*)malloc(velodromo->total_ciclistas*sizeof(nodulo));
	for (i = 0; i < velodromo->total_ciclistas; i++) ranking_volta[i] = meu_placar->ranking_geral[volta-2][i];
	qsort(ranking_volta, velodromo->total_ciclistas, sizeof(nodulo), compara_nodulos);
	return ranking_volta;
}
