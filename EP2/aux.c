//Allan Amancio Rocha - 9761614
//Igor Fratel Santana - 9793565

#include "aux.h"

//fonts color
#define FBLACK      "\033[30;"
#define FRED        "\033[31;"

//background color
#define BWHITE      "47m"

//end color
#define NONE        "\033[0m"


int mod(int a, int b) {
	/*Returns a%b*/
	if(b < 0)
		return mod(a, -b);
	int ret = a % b;
	if(ret < 0)
		ret+=b;
	return ret;
}

void imprime_pista() {
	int i, j, k;
	int num_digitos;
	num_digitos = conta_digitos(velodromo->total_ciclistas);
	for (i = 0; i < 10; i++) {
		for (j = 0; j < velodromo->tamanho; j++) {
			if (velodromo->circuito[i][j]) {
				for (k = num_digitos - conta_digitos(velodromo->circuito[i][j]); k > 0; k--)
					printf(FBLACK BWHITE " ");
				printf(FRED BWHITE " %d "NONE, velodromo->circuito[i][j]);
			}
			else {
				for (k = num_digitos - 1; k > 0; k--)
					printf(FBLACK BWHITE " ");
				printf(FBLACK BWHITE " - ");
			}
			if (j == velodromo->tamanho - 1)
				printf(""NONE);
		}
		printf("\n");
	}
	printf("\n\n");
}

void imprime_placar() {
	int i;
	ordena_placar();
	printf("PLACAR\n");
	for (i = 0; i < velodromo->total_ciclistas; i++) {
		if (meu_placar->ranking[i]->quebrado) break;
		printf("(%d, %d) ", meu_placar->ranking[i]->id, meu_placar->ranking[i]->pontos);
	}
	printf("\n\n");
}

void imprime_final() {
	int i, j, num_digitos;
	ciclista *c_imprime;
	ordena_placar();
	if (velodromo->total_ciclistas > meu_placar->ranking[0]->pontos)
		num_digitos = conta_digitos(velodromo->total_ciclistas);
	else num_digitos = conta_digitos(meu_placar->ranking[0]->pontos);
	for (i = 0; i < velodromo->total_ciclistas; i++) {
		c_imprime = meu_placar->ranking[i];
		if (!meu_placar->ranking[i]->quebrado) {
			printf("ID: ");
			for (j = num_digitos - conta_digitos(c_imprime->id); j > 0; j--) printf(" ");
			printf("%d", c_imprime->id);
			printf(" Pontuação final: ");
			for (j = num_digitos - conta_digitos(c_imprime->pontos); j > 0; j--) printf(" ");
			printf("%d", c_imprime->pontos);
			printf(" Tempo de chegada: ");
			for (j = num_digitos - conta_digitos(c_imprime->termino_corrida); j > 0; j--) printf(" ");
			printf("%d\n", c_imprime->termino_corrida);
		}
		else {
			printf("ID: ");
			for (j = num_digitos - conta_digitos(c_imprime->id); j > 0; j--) printf(" ");
			printf("%d", c_imprime->id);
			printf(" Ciclista quebrou na volta: ");
			for (j = num_digitos - conta_digitos(c_imprime->quebrado); j > 0; j--) printf(" ");
			printf("%d\n", c_imprime->quebrado);
		}
	}
}

void aumenta_relogio() {
	if (velodromo->v_max != 90) relogio += 60;
	else relogio += 20;

	// Debug
	if (debug) {
		pthread_mutex_lock(&mutex_pista);
		imprime_pista();
		pthread_mutex_unlock(&mutex_pista);
	}
}

int conta_digitos(int a) {
	int num_digitos;
	for (num_digitos = 0; a > 0; num_digitos++)
		a = a/10;

	if (num_digitos == 0) num_digitos++;

	return num_digitos;
}

#define cast_c1 (*(ciclista **)c1)
#define cast_c2 (*(ciclista **)c2)

int compara_ciclistas(const void *c1, const void *c2) {
	if (cast_c1->quebrado) {
		if (cast_c2->quebrado) return (cast_c2->pontos - cast_c1->pontos); //c1 e c2 quebrados
		else return 1; //apenas c1 quebrado
	}
	else if (cast_c2->quebrado) return -1; //apenas c2 quebrado
	else return (cast_c2->pontos - cast_c1->pontos); //nenhum quebrado;
}

void espera_ciclistas() {
	int i;
	int *threads;
	threads = malloc(velodromo->total_ciclistas*sizeof(int));
	for (i = 0; i < velodromo->total_ciclistas; i++) {
		pthread_mutex_lock(&mutex_placar);
		threads[i] = meu_placar->ranking[i]->thread_id;
		pthread_mutex_unlock(&mutex_placar);
	}
	//Espera as threads encerrarem
	for (i = 0; i < velodromo->total_ciclistas; i++) {
		pthread_join(threads[i], NULL);
	}
}

#define cast_nodulo1 (*(nodulo *)nodulo1)
#define cast_nodulo2 (*(nodulo *)nodulo2)

int compara_nodulos(const void *nodulo1, const void *nodulo2) {
	return (cast_nodulo2.pontos_acumulado - cast_nodulo1.pontos_acumulado);
}

int posicao_ciclista(int id, int volta) {
	int i;
	for (i = 0; i < velodromo->total_ciclistas; i++)
		if (meu_placar->ranking_geral[volta-2][i].id == id) {
			return i + 1;
		}
	
	return 0;
}

void imprime_ciclistas(int volta) {
	int i, m_10 = 0, posicao = 1; //Múltiplo de 10
	nodulo *ciclistas_pontos = NULL;
	if (volta%10 == 1) {
		m_10 = 1;
		ordena_placar();
		ciclistas_pontos = ordena_ranking_geral(volta);
	}
	for (i = 0, posicao = 1; i < velodromo->total_ciclistas; i++) {
		if (meu_placar->ranking_geral[volta-2][i].id == -1) {
			if (m_10) break;
			else continue;
		}

		if (m_10) printf("ID: %d Posicao: %d Pontuação acumulada: %d\n", ciclistas_pontos[i].id, posicao_ciclista(ciclistas_pontos[i].id, volta), ciclistas_pontos[i].pontos_acumulado);
		else printf("ID: %d Posicao: %d\n", meu_placar->ranking_geral[volta-2][i].id, posicao);
		
		posicao++;
	}
	printf("\n");
	
	if (ciclistas_pontos) free(ciclistas_pontos);
}
