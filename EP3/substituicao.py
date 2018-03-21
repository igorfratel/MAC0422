#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

import math

#substituicao

def optimal(total, p, mmu, tarefas, t_indice):
	#busca o primeiro quadro de página livre, se houver:
	memoria_fisica = [-1]*int(total/p)
	for pagina in mmu:
		if (pagina.endereco_fisico == -1): continue
		quadro_pagina = int(pagina.endereco_fisico/p)
		memoria_fisica[quadro_pagina] = pagina.processo

	i = 0
	while (i < len(memoria_fisica)):
		if (memoria_fisica[i] == -1): break
		else: i += 1

	endereco_fisico = i*p
	if (endereco_fisico < total):
		#caso em que houve quadro de página livre
		return endereco_fisico
	else:
		#escolha de um quadro de página a ser retirado
		quadro_otimo = None
		quadros_verificados = []
		for i in range(0, len(memoria_fisica)): quadros_verificados.append(0)

		for i in range(t_indice + 1, len(tarefas)):
			if (tarefas[i].codigo == "acessa"):
				for pagina in mmu:
					if (pagina.processo == tarefas[i].id and pagina.posicao == tarefas[i].posicao and pagina.endereco_fisico != -1):
						q = int(pagina.endereco_fisico/p)
						if (quadros_verificados[q] == 0):
							quadros_verificados[q] = 1
							quadro_otimo = pagina
						break
			else: continue

		#Quadro não verificado
		for i in range(0, len(quadros_verificados)):
			if (quadros_verificados[i] == 0):
				for pagina in mmu:
					if (pagina.endereco_fisico == i*p):
						endereco_fisico = pagina.endereco_fisico
						pagina.endereco_fisico = -1
						return endereco_fisico
				
		#Melhor quadro verificado
		if (quadro_otimo is not None):
			endereco_fisico = quadro_otimo.endereco_fisico
			quadro_otimo.endereco_fisico = -1
			return endereco_fisico

		#um quadro qualquer (pois, não há quadro a ser escolhido pelo algoritmo)
		for pagina in mmu:
			if (pagina.processo != -1 and pagina.endereco_fisico != -1):
				endereco_fisico = pagina.endereco_fisico
				pagina.endereco_fisico = -1
				return endereco_fisico

def fifo(total, p, mmu, fila_quadros):
    #busca o primeiro quadro de página livre, se houver:
    memoria_fisica = [-1]*int(total/p)
    for pagina in mmu:
        if (pagina.endereco_fisico == -1): continue
        quadro_pagina = int(pagina.endereco_fisico/p)
        memoria_fisica[quadro_pagina] = pagina.processo

    i = 0
    while (i < len(memoria_fisica)):
        if (memoria_fisica[i] == -1): break
        else: i += 1

    endereco_fisico = i*p
    if (endereco_fisico < total):
        #caso em que houve quadro de página livre
        if (endereco_fisico not in fila_quadros): fila_quadros.append(endereco_fisico)
        return endereco_fisico
    else:
        #escolha de um quadro de página a ser retirado
        for i in range(0, len(fila_quadros)):
            endereco_fisico = fila_quadros.pop(i)
            if (endereco_fisico != -1):
                fila_quadros.append(endereco_fisico)
                for pagina in mmu:
                    if (pagina.endereco_fisico == endereco_fisico):
                        pagina.endereco_fisico = -1
                return endereco_fisico

def lru_2(total, p, mmu, matriz_quadros):
    #busca o primeiro quadro de página livre, se houver:
    n = len(matriz_quadros[0])

    memoria_fisica = [-1]*int(total/p)
    for pagina in mmu:
        if (pagina.endereco_fisico == -1): continue
        quadro_pagina = int(pagina.endereco_fisico/p)
        memoria_fisica[quadro_pagina] = pagina.processo

    q = 0
    while (q < len(memoria_fisica)):
        if (memoria_fisica[q] == -1): break
        else: q += 1

    endereco_fisico = q*p
    if (endereco_fisico < total):
        #caso em que houve quadro de página livre
        for i in range(0, n):
            matriz_quadros[q][i] = 1
        for i in range(0, n):
            matriz_quadros[i][q] = 0
        return endereco_fisico
    else:
        #escolha de um quadro de página a ser retirado
        for q_escolhido in range(0, n):
            coluna = 0
            while (coluna < n):
                if (matriz_quadros[q_escolhido][coluna] == 1): break
                else: coluna += 1
            if (coluna == n):
                for i in range(0, n):
                    matriz_quadros[q_escolhido][i] = 1
                for i in range(0, n):
                    matriz_quadros[i][q_escolhido] = 0
                endereco_fisico = q_escolhido*p
                for pagina in mmu:
                    if (pagina.endereco_fisico == endereco_fisico):
                        pagina.endereco_fisico = -1
                return endereco_fisico

def lru_4(total, p, mmu, contador_quadros):
    #busca o primeiro quadro de página livre, se houver:
    memoria_fisica = [-1]*int(total/p)
    for pagina in mmu:
        if (pagina.endereco_fisico == -1): continue
        quadro_pagina = int(pagina.endereco_fisico/p)
        memoria_fisica[quadro_pagina] = pagina.processo

    q = 0
    while (q < len(memoria_fisica)):
        if (memoria_fisica[q] == -1): break
        else: q += 1

    endereco_fisico = q*p
    if (endereco_fisico < total):
        #caso em que houve quadro de página livre
        return endereco_fisico
    else:
        contador_velho = math.inf
        for q in range(0, len(contador_quadros)):
            if (contador_quadros[q] < contador_velho):
                contador_velho = contador_quadros[q]
                quadro_velho = q

        endereco_fisico = quadro_velho*p
        for pagina in mmu:
            if (pagina.endereco_fisico == endereco_fisico):
                pagina.endereco_fisico = -1
        return endereco_fisico
