#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

import substituicao
import espaco
from Tarefa import Tarefa
from Pagina import Pagina
from Node import Node
from memoria import aloca_memoria, desaloca_memoria, compacta, gera_lista_quickfit, atualiza_memoriafisica
from aux import imprime_memoria
from paginacao import insere_pagina, remove_pagina

def simulacao(intervalo, fun_espaco, fun_substitui, total, virtual, s, p, tarefas):
    #getattr(substituicao, fun_substitui)() <--- para chamar funcoes a partir dos args
    INTERVALO_R = 3 # Intervalo de tempo para zerar os bits R da mmu

    tempo = 0;
    t_indice = 0 #indice do vetor tarefas a ser acessado na iteração atual
    bitmap = Node("L", -1, -1, 0, int(virtual/s)) #Lista ligada, cada posição contém uma tripla (P ou L (estado), base, deslocamento). Memória física
    mmu = [Pagina() for i in range(int(virtual/p))]

    #Variáveis condicionais
    #relativas aos algoritmos de espaço
    if (fun_espaco == "quick_fit"):
        lista_quickfit = gera_lista_quickfit(tarefas, bitmap, s, p)  #Será usada caso o algoritmo de espaço livre seja o quick-fit
    else: lista_quickfit = None #dummy

    #relativas aos algoritmos de substituição
    if (fun_substitui == "fifo"):
        fila_quadros = [] #Fila de quadros de páginas que chegaram mais recentemente na memória física
    elif (fun_substitui == "lru_2"):
        matriz_quadros = []
        for i in range(0, int(total/p)):
            matriz_quadros.append([])
            for j in range(0, int(total/p)):
                matriz_quadros[i].append(0)
    elif (fun_substitui == "lru_4"):
        contador_quadros = [] #contadores de 8 bits de cada quadro de página
        for i in range(0, int(total/p)):
            contador_quadros.append(0)

    arquivo_virtual = open("/tmp/ep3.vir", 'w+') #bytes
    arquivo_fisico = open("/tmp/ep3.mem", 'w+') #bytes

    #Preencher arquivos com "-1"
    for i in range(0, virtual): arquivo_virtual.write("10000001\n")
    for i in range(0, total): arquivo_fisico.write("10000001\n")

    arquivo_fisico.close()
    arquivo_virtual.close()

    while(t_indice < len(tarefas)): #Cada loop é um segundo
        if (tempo%intervalo == 0): imprime_memoria(bitmap, mmu, s, tempo)
        if (tempo%INTERVALO_R == 0): #zera bits R da mmu
            for pagina in mmu: pagina.r = 0
        if (fun_substitui == "lru_4"): #atualiza contadores do lru_4
            for pagina in mmu:
                if (pagina.endereco_fisico != -1):
                    quadro = int(pagina.endereco_fisico/p)
                    contador_quadros[quadro] = contador_quadros[quadro]/2 #desloca 1 bit para direita
                    contador_quadros[quadro] += pagina.r*2**7 #adiciona o bit R no bit mais significativo

        while(t_indice < len(tarefas) and tempo == tarefas[t_indice].tempo):
            #Executa tarefa
            if(tarefas[t_indice].codigo == "cria"):
                bitmap, lista_quickfit = getattr(espaco, fun_espaco)(bitmap, tarefas[t_indice].id, tarefas[t_indice].b, s, p, lista_quickfit)
                insere_pagina(bitmap, tarefas[t_indice].id, mmu, s, p)
                aloca_memoria(bitmap, s)

            elif(tarefas[t_indice].codigo == "destroi"):
                remove_pagina(bitmap, tarefas[t_indice].id, mmu, s, p)
                bitmap, lista_quickfit = desaloca_memoria(bitmap, tarefas[t_indice].id, s, p, mmu, lista_quickfit)
                atualiza_memoriafisica(bitmap, mmu, total, p)

            elif(tarefas[t_indice].codigo == "acessa"):
                for pagina in mmu: #o acesso só ocorre porque o processo juntamente com suas páginas existe
                    if (pagina.processo == tarefas[t_indice].id and pagina.posicao == tarefas[t_indice].posicao):
                        if (pagina.endereco_fisico == -1): #PAGE FAULT
                            if (fun_substitui == "optimal"):
                                pagina.endereco_fisico = substituicao.optimal(total, p, mmu, tarefas, t_indice)
                            elif (fun_substitui == "fifo"):
                                pagina.endereco_fisico = substituicao.fifo(total, p, mmu, fila_quadros)
                            elif (fun_substitui == "lru_2"):
                                pagina.endereco_fisico = substituicao.lru_2(total, p, mmu, matriz_quadros)
                            else: #fun_substitui == "lru_4"
                                pagina.endereco_fisico = substituicao.lru_4(total, p, mmu, contador_quadros)

                            atualiza_memoriafisica(bitmap, mmu, total, p)
                        pagina.r = 1
                        break

            elif(tarefas[t_indice].codigo == "compacta"):
                bitmap, lista_quickfit = compacta(bitmap, s, p, total, mmu, lista_quickfit)
                atualiza_memoriafisica(bitmap, mmu, total, p)

            t_indice += 1
        tempo += 1

    imprime_memoria(bitmap, mmu, s, tempo)
