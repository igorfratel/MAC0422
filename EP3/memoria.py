import math
from Node import Node

def aloca_memoria(bitmap, s):
    #Reescreve o arquivo ./tmp/ep3.vir a partir do bitmap
    atual = bitmap

    with open("/tmp/ep3.vir", 'w') as f:
        while(atual is not None):
            if (atual.estado == "L"):
                for i in range(0, int(atual.deslocamento*s)): f.write("10000001\n")

            else:
                for i in range(0, atual.tamanho_processo): f.write('{0:08b}'.format(atual.processo) + "\n")
                for i in range(atual.tamanho_processo, int(atual.deslocamento*s)): f.write("10000001\n")

            atual = atual.prox

def desaloca_memoria(bitmap, processo, s, p, mmu, lista_quickfit):
    #Modifica o bitmap retirando um processo e reescreve o arquivo ./tmp/ep3.vir a partir do bitmap
    atual = bitmap
    ant = None
    ant2 = None #antepenultimo

    while(atual is not None):
        if(atual.processo == processo):
            #achei o processo

            if(ant is not None and ant.estado == "L"):
                #anterior é livre
                if (lista_quickfit is not None): poda_lista_quickfit(bitmap, ant2, s, p, lista_quickfit)

                if(atual.prox is not None and atual.prox.estado == "L"):
                    #anterior e proximo são livres
                    if (lista_quickfit is not None): poda_lista_quickfit(bitmap, atual, s, p, lista_quickfit)

                    novo_livre = Node("L", -1, -1, ant.base, ant.deslocamento + atual.deslocamento + atual.prox.deslocamento)
                    novo_livre.prox = atual.prox.prox

                    tamanho_processo = novo_livre.deslocamento*s
                    paginas = math.floor(tamanho_processo/p)
                    if (ant2 is not None):
                        ant2.prox = novo_livre
                        if (lista_quickfit is not None):
                            lista_quickfit = atualiza_lista_quickfit(ant2, paginas, lista_quickfit)
                    else:
                        bitmap = novo_livre
                        if (lista_quickfit is not None):
                            lista_quickfit = atualiza_lista_quickfit(None, paginas, lista_quickfit)

                else:
                    #anterior é livre
                    novo_livre = Node("L", -1, -1, ant.base, ant.deslocamento + atual.deslocamento)
                    novo_livre.prox = atual.prox

                    tamanho_processo = novo_livre.deslocamento*s
                    paginas = math.floor(tamanho_processo/p)
                    if (ant2 is not None):
                        ant2.prox = novo_livre
                        if (lista_quickfit is not None):
                            lista_quickfit = atualiza_lista_quickfit(ant2, paginas, lista_quickfit)
                    else:
                        bitmap = novo_livre
                        if (lista_quickfit is not None):
                            lista_quickfit = atualiza_lista_quickfit(None, paginas, lista_quickfit)

            else:
                #anterior é ocupado ou não existe
                if(atual.prox is not None and atual.prox.estado == "L"):
                    #proximo é livre
                    if (lista_quickfit is not None): poda_lista_quickfit(bitmap, atual, s, p, lista_quickfit)

                    novo_livre = Node("L", -1, -1, atual.base, atual.prox.deslocamento + atual.deslocamento)
                    novo_livre.prox = atual.prox.prox

                    tamanho_processo = novo_livre.deslocamento*s
                    paginas = math.floor(tamanho_processo/p)
                    if (ant is not None):
                        ant.prox = novo_livre
                        if (lista_quickfit is not None):
                            lista_quickfit = atualiza_lista_quickfit(ant, paginas, lista_quickfit)
                    else:
                        bitmap = novo_livre
                        if (lista_quickfit is not None):
                            lista_quickfit = atualiza_lista_quickfit(None, paginas, lista_quickfit)

                else:
                    #todos ocupados
                    atual.estado = "L"
                    atual.processo = -1
                    atual.tamanho_processo = -1

                    tamanho_processo = atual.deslocamento*s
                    paginas = math.floor(tamanho_processo/p)
                    if (lista_quickfit is not None):
                        if (ant is not None):
                            lista_quickfit = atualiza_lista_quickfit(ant, paginas, lista_quickfit)
                        else:
                            lista_quickfit = atualiza_lista_quickfit(None, paginas, lista_quickfit)

            break

        ant2 = ant
        ant = atual
        atual = atual.prox

    atual = bitmap
    with open("/tmp/ep3.vir", 'w') as f:
        while(atual is not None):
            if (atual.estado == "L"):
                for i in range(0, int(atual.deslocamento*s)): f.write("10000001\n")

            else:
                for i in range(0, atual.tamanho_processo): f.write('{0:08b}'.format(atual.processo) + "\n")
                for i in range(atual.tamanho_processo, int(atual.deslocamento*s)): f.write("10000001\n")

            atual = atual.prox

    return bitmap, lista_quickfit

def compacta(bitmap, s, p, total, mmu, lista_quickfit):
    #--------------------------------------------------MEMÓRIA VIRTUAL----------------------------------------------
    deslocamento_acumulado = 0
    atual = bitmap
    ant = None
    ant2 = None

    while(atual is not None):
        if(atual.estado == "L"):
            deslocamento_acumulado += atual.deslocamento
            if(ant is not None): ant.prox = atual.prox
            else: bitmap = atual.prox

        else:
            if(ant is not None):
                if (ant.estado == "P"): atual.base = ant.base + ant.deslocamento
                elif (ant2 is not None and ant2.estado == "P"): atual.base = ant2.base + ant2.deslocamento #ant.estado == "L"
                else: atual.base = 0 #ant2 is None

            else: atual.base = 0 #compacta a cabeça da lista

        ant2 = ant
        ant = atual
        atual = atual.prox

    if (lista_quickfit is not None):
        for lista in lista_quickfit:
            while (lista[1]): lista[1].pop() #zera listas

    tamanho_processo = deslocamento_acumulado*s
    paginas = math.floor(tamanho_processo/p)
    if (ant.estado == "P"):
        novo_livre = Node("L", -1, -1, ant.base + ant.deslocamento, deslocamento_acumulado)
        ant.prox = novo_livre
        if (lista_quickfit is not None):
            lista_quickfit = atualiza_lista_quickfit(ant, paginas, lista_quickfit)
    elif (ant2 is not None):
        novo_livre = Node("L", -1, -1, ant2.base + ant2.deslocamento, deslocamento_acumulado)
        ant2.prox = novo_livre
        if (lista_quickfit is not None):
            lista_quickfit = atualiza_lista_quickfit(ant2, paginas, lista_quickfit)
    else:
        novo_livre = Node("L", -1, -1, 0, deslocamento_acumulado)
        bitmap = novo_livre
        if (lista_quickfit is not None):
            lista_quickfit = atualiza_lista_quickfit(None, paginas, lista_quickfit)

    #Atualização da memória virtual
    atual = bitmap
    with open("/tmp/ep3.vir", 'w') as f:
        while(atual is not None):
            if (atual.estado == "L"):
                for i in range(0, int(atual.deslocamento*s)): f.write("10000001\n")

            else:
                for i in range(0, atual.tamanho_processo): f.write('{0:08b}'.format(atual.processo) + "\n")
                for i in range(atual.tamanho_processo, int(atual.deslocamento*s)): f.write("10000001\n")

            atual = atual.prox

    #--------------------------------------------------------MMU---------------------------------------------------
    for i in range(0, len(mmu)):
        if (mmu[i].processo == -1): continue
        novo_i = i
        for j in range(0, i):
            if(mmu[j].processo == -1):
                novo_i = j
                break

        mmu[novo_i], mmu[i] = mmu[i], mmu[novo_i]

    #--------------------------------------------------MEMÓRIA FÍSICA----------------------------------------------
    memoria_fisica = [-1]*int(total/p)
    for pagina in mmu:
        if (pagina.endereco_fisico == -1): continue
        quadro_pagina = int(pagina.endereco_fisico/p)
        memoria_fisica[quadro_pagina] = pagina.processo

    for pagina in mmu:
        if (pagina.endereco_fisico == -1): continue
        velho_quadro = int(pagina.endereco_fisico/p)
        novo_quadro = velho_quadro
        for j in range(0, velho_quadro):
            if(memoria_fisica[j] == -1):
                novo_quadro = j
                break

        memoria_fisica[novo_quadro], memoria_fisica[velho_quadro] = memoria_fisica[velho_quadro], memoria_fisica[novo_quadro]
        pagina.endereco_fisico = novo_quadro*p

    return bitmap, lista_quickfit

def atualiza_memoriafisica(bitmap, mmu, total, p):
    memoria_fisica = ["10000001"]*total
    
    atual = bitmap
    total_paginas = atual.tamanho_processo/p
    for pagina in mmu:
        if (pagina.processo == -1 or pagina.endereco_fisico == -1): continue

        if (atual.processo != pagina.processo):
            atual = bitmap
            while (atual is not None):
                if (atual.processo == pagina.processo):
                    break
                atual = atual.prox
            total_paginas = atual.tamanho_processo/p

        if (pagina.posicao <= total_paginas): pn = p
        else: pn = int(-1*(total_paginas - pagina.posicao)*p)
        for i in range(0, pn):
            memoria_fisica[pagina.endereco_fisico + i] = '{0:08b}'.format(pagina.processo)

    with open("/tmp/ep3.mem", 'w') as f:
        for byte in memoria_fisica:
            f.write(byte + "\n")

def gera_lista_quickfit(tarefas, bitmap, s, p):
    #Gera lista de tamanhos mais requisitados em forma de páginas para o quick_fit
    lista_tamanhos = [] #lista de todos os tamanhos requisitados
    lista_quickfit = [] #lista dos tamanhos mais requisitados

    #preenche o lista_tamanhos baseado em todos os processos a serem criados
    for tarefa in tarefas:
        if (tarefa.codigo == "cria"):
            tamanho_processo = math.ceil(tarefa.b/s)*s #quantidade de bytes alocado para o processo
            paginas = math.ceil(tamanho_processo/p) #quantidade de páginas necessárias para o processo

            encontrado = 0
            for tamanho in lista_tamanhos:
                if (tamanho[0] == paginas):
                    tamanho[1] += 1
                    encontrado = 1
                    break
            if (encontrado == 0):
                lista_tamanhos.append([paginas, 1])

    #ordenação por frequência de tamanhos requisitados
    lista_tamanhos = sorted(lista_tamanhos, key=lambda lista_tamanhos: lista_tamanhos[1])

    #top 5
    i = len(lista_tamanhos) - 1
    for top in range(0, 5):
        if (i >= 0):
            lista_quickfit.append((lista_tamanhos[i][0], []))
            i -= 1
        else:
            break

    #ordenação do top 5
    lista_quickfit = sorted(lista_quickfit, key=lambda lista_quickfit: lista_quickfit[0])

    #preenche as listas do top 5
    tamanho_processo = bitmap.deslocamento*s
    paginas = tamanho_processo/p
    lista_quickfit = atualiza_lista_quickfit(None, paginas, lista_quickfit)

    return lista_quickfit

def atualiza_lista_quickfit(bloco_ant, numero_paginas, lista_quickfit):
    #Procura se na lista_quickfit tem lista para numero_paginas de tamanho requisitado
    #Se tiver, acrescenta o bloco_ant (bloco anterior ao bloco livre) na lista
    for tamanho in lista_quickfit:
        if (tamanho[0] == numero_paginas):
            tamanho[1].append(bloco_ant)

    return lista_quickfit

def poda_lista_quickfit(bitmap, bloco_ant_livre, s, p, lista_quickfit):
    #Verifica se o bloco_livre está na lista_quickfit e o remove
    if (bloco_ant_livre is not None): paginas = int(bloco_ant_livre.prox.deslocamento*s/p)
    else: paginas = int(bitmap.deslocamento*s/p)
    for lista in lista_quickfit:
        if (lista[0] == paginas):
            for i in range(0, len(lista[1])):
                if (lista[1][i] == bloco_ant_livre):
                    lista[1].pop(i)
                    break
