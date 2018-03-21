import math
from Node import Node
from memoria import atualiza_lista_quickfit

#Algoritmos de gerenciamento de espaço livre

def best_fit(bitmap, processo, tamanho_processo, s, p, lista_quickfit):
    #Algoritmo Best Fit
    #Modifica o bitmap com o novo processo
    melhor = Node("L", -1, -1, 0, math.inf) #melhor índice da lista ligada
    atual = bitmap
    ant = None
    melhor_ant = None

    tamanho_processo = math.ceil(tamanho_processo/s)*s #quantidade de bytes alocado para o processo
    paginas = math.ceil(tamanho_processo/p) #quantidade de páginas necessárias para o processo

    while(atual is not None):
        if (atual.estado == "L" and atual.deslocamento*s >= paginas*p and atual.deslocamento < melhor.deslocamento):
            melhor_ant = ant
            melhor = atual
            if (melhor.deslocamento*s == paginas*p): break
        ant = atual
        atual = atual.prox

    if (paginas*p == melhor.deslocamento*s):
        melhor.estado = "P"
        melhor.processo = processo
        melhor.tamanho_processo = tamanho_processo

    else:
        novo_processo = Node("P", processo, tamanho_processo, melhor.base, (paginas*p)/s)
        if (melhor_ant is not None): melhor_ant.prox = novo_processo
        else: bitmap = novo_processo #nova cabeça
        
        novo_livre = Node("L", -1, -1, melhor.base + (paginas*p)/s, melhor.deslocamento - (paginas*p)/s) #memória livre restante
        novo_processo.prox = novo_livre
        
        novo_livre.prox = melhor.prox #reestruturando a lista ligada

        if (lista_quickfit is not None):
            #atualização do quick-fit
            tamanho_processo = novo_livre.deslocamento*s
            paginas = math.floor(tamanho_processo/p)
            lista_quickfit = atualiza_lista_quickfit(novo_processo, paginas, lista_quickfit)

    return bitmap, lista_quickfit

def worst_fit(bitmap, processo, tamanho_processo, s, p, lista_quickfit):
    #Algoritmo Best Fit
    #Modifica o bitmap com o novo processo
    maior = Node("L", -1, -1, 0, 0) #melhor índice da lista ligada
    atual = bitmap
    ant = None
    maior_ant = None

    tamanho_processo = math.ceil(tamanho_processo/s)*s #quantidade de bytes alocado para o processo
    paginas = math.ceil(tamanho_processo/p) #quantidade de páginas necessárias para o processo

    while(atual is not None):
        if (atual.estado == "L" and atual.deslocamento*s >= paginas*p and atual.deslocamento > maior.deslocamento):
            maior_ant = ant
            maior = atual
        ant = atual
        atual = atual.prox

    if (paginas*p == maior.deslocamento*s):
        maior.estado = "P"
        maior.processo = processo
        maior.tamanho_processo = tamanho_processo
    else:
        novo_processo = Node("P", processo, tamanho_processo, maior.base, (paginas*p)/s)
        if (maior_ant is not None): maior_ant.prox = novo_processo
        else: bitmap = novo_processo #nova cabeça
        
        novo_livre = Node("L", -1, -1, maior.base + (paginas*p)/s, maior.deslocamento - (paginas*p)/s) #memória livre restante
        novo_processo.prox = novo_livre
        
        novo_livre.prox = maior.prox #reestruturando a lista ligada

        if (lista_quickfit is not None):
            #atualização do quick-fit
            tamanho_processo = novo_livre.deslocamento*s
            paginas = math.floor(tamanho_processo/p)
            lista_quickfit = atualiza_lista_quickfit(novo_processo, paginas, lista_quickfit)

    return bitmap, lista_quickfit

def quick_fit(bitmap, processo, tamanho_processo, s, p, lista_quickfit):
    tamanho_processo = math.ceil(tamanho_processo/s)*s #quantidade de bytes alocado para o processo
    paginas = math.ceil(tamanho_processo/p) #quantidade de páginas necessárias para o processo

    quickfit_on = 0
    #busca do bloco livre
    for tamanho in lista_quickfit:
        if (tamanho[0] >= paginas and len(tamanho[1]) > 0):
            bloco_ant = tamanho[1].pop() #bloco anterior ao bloco livre
            if (bloco_ant is not None): bloco_livre = bloco_ant.prox
            else: bloco_livre = bitmap
            quickfit_on = 1
            break

    if (quickfit_on == 0): #fracasso do quickfit
        bitmap, lista_quickfit = worst_fit(bitmap, processo, tamanho_processo, s, p, lista_quickfit)
    else: #êxito do quickfit
        if (paginas*p == bloco_livre.deslocamento*s):
            bloco_livre.estado = "P"
            bloco_livre.processo = processo
            bloco_livre.tamanho_processo = tamanho_processo
        else:
            novo_processo = Node("P", processo, tamanho_processo, bloco_livre.base, (paginas*p)/s)
            if (bloco_ant is not None): bloco_ant.prox = novo_processo
            else: bitmap = novo_processo #nova cabeça
            
            novo_livre = Node("L", -1, -1, bloco_livre.base + (paginas*p)/s, bloco_livre.deslocamento - (paginas*p)/s) #memória livre restante
            novo_processo.prox = novo_livre
            
            novo_livre.prox = bloco_livre.prox #reestruturando a lista ligada

            #atualização do quick-fit
            tamanho_processo = novo_livre.deslocamento*s
            paginas = math.floor(tamanho_processo/p)
            lista_quickfit = atualiza_lista_quickfit(novo_processo, paginas, lista_quickfit)

    return bitmap, lista_quickfit
