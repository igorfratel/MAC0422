#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

import random as rand
import math

#limites:
#s --> 1 <= s <= 10
#p --> s ou 2*s
#virtual --> p <= virtual <= 1000 (múltiplo de p)
#total --> p <= total <= virtual (múltiplo de p)

#tf --> t0 < tf <= 1800
#t0 --> 0 <= t0 < tf
#Os processos vêm ordenados por t0 em ordem não-decrescente
#b --> 1 <= b <= virtual

def gera_trace():
    rand.seed()

    TEMPO_SIMULA = 900
    #(start) Cuida da primeira linha do trace
    #total virtual s p
    s = rand.randint(1, 10)
    p = rand.randint(1, 20)
    p = math.ceil(p/s)*s
    virtual = rand.randint(1, math.floor(1000/p))*p
    total = rand.randint(1, int(virtual/p))*p
    print(str(total) + " " + str(virtual) + " " + str(s) + " " + str(p))
    #(end) Cuida da primeira linha do trace

    #(start) Cuida das linhas que contém processos
    #t0 tf b nome p1 t1 p2 t2 p3 t3 [pn tn]
    t0 = 0
    # restante_global = virtual #começa com virtual e é diminuido sempre que um novo processo é criado
    restante_global = virtual #total de memória disponível na iteração atual, contando apenas os processos rodando no momento
    total_processos = rand.randint(1, 128)
    processos_rodando = [] #contém triplas do tipo (tf, b, t0)
    tamanho_medio = math.ceil(virtual/total_processos)
    tempo_medio = math.ceil(TEMPO_SIMULA/total_processos)
    variancia = tamanho_medio/2

    for i in range(0, total_processos):
        nome = "processo" + str(i)
        t0 = rand.randint(t0, int((TEMPO_SIMULA-1)/(total_processos-i)))
        if (rand.randint(1, 100) > 5):
            tf = rand.randint(t0 + 1, min(t0 + tempo_medio, TEMPO_SIMULA))
        else:
            tf = rand.randint(t0 + 1, max(t0 + tempo_medio, TEMPO_SIMULA))

        #Percorre a processos_rodando e retira todos os processos que já morreram
        k = 0
        while(k < len(processos_rodando)):
            if processos_rodando[k][0] < t0:
                restante_global += processos_rodando[k][1]
                processos_rodando.pop(k)
            else: k += 1

        #Lidando com a geração de tamanhos
        if (math.floor(restante_global/p) < 1):
            continue
        else:
            if (rand.randint(1, 100) > 5):
                b = rand.randint(1, min(int(tamanho_medio + variancia), math.floor(restante_global/p)*p))
            else:
                b = rand.randint(1, math.floor(restante_global/p)*p)
            b_real = math.ceil(b/p)*p

        #Determina a memória disponivel no momento fazendo virtual - soma dos tamanhos de todos os processos rodando (sem contar o que criamos agora)

        #(start) Cuida das linhas com compactar
        #t COMPACTAR
        if b_real > math.ceil(restante_global/(len(processos_rodando) + 1)):
            if (tf_ant > t0): t = rand.randint(t0_ant, t0)
            else: t = rand.randint(tf_ant, t0)
            print(str(t) + " COMPACTAR")
        #(end) Cuida das linhas com compactar

        #Adiciona o novo processo na lista
        processos_rodando.append((tf, b_real, t0))
        restante_global -= b_real

        #(start) Cuida dos acessos
            #p1 t1 ... pi ti ... pn tn
            #t0 <= t1 < ti < tn <= tf
            #tamanhodoprocesso = math.ceil(b/s)*s
            #1 <= pi <= math.ceil(tamanhodoprocesso/p)
            #1 <= numero de acessos <= tf-t0
        acessos = [] #(p, t)
        p1 = -1
        pn = int(b_real/p)
        total_acessos = min(tf-t0, 15)
        intervalo = int((tf - t0)/total_acessos)
        for j in range(0, total_acessos):
            coin = rand.randint(0, 1)
            if coin == 1:
                #Gera acesso a página
                if (p1 == -1):
                    pi = 1
                    p1 = 0
                else:
                    pi = rand.randint(1, pn)
                t = rand.randint(j*intervalo, (j + 1)*intervalo)
                acessos.append((pi, t0 + t))
        #(end) Cuida dos acessos
        string_acessos = ""
        for j in range(0, len(acessos)):
            string_acessos += " " + str(acessos[j][0]) + " " + str(acessos[j][1])
        print(str(t0) + " " + str(tf) + " " + str(b) + " " + nome + string_acessos)
        #(end) Cuida das linhas que contém processos

        t0_ant = t0
        tf_ant = tf

def main():
    gera_trace()


if __name__ == "__main__":
    main()
