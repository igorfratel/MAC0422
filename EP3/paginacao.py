#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

#Funções auxiliares para paginação e mmu

def insere_pagina(bitmap, processo, mmu, s, p):
    atual = bitmap
    while (atual is not None):
        if (atual.processo == processo):
            break
        atual = atual.prox

    p_indice = int(atual.base*s/p)
    num_paginas = int(atual.deslocamento*s/p)

    posicao = 1
    for i in range(p_indice, p_indice + num_paginas):
        mmu[i].processo = processo
        mmu[i].posicao = posicao
        posicao += 1

def remove_pagina(bitmap, processo, mmu, s, p):
    atual = bitmap
    while (atual is not None):
        if (atual.processo == processo):
            break
        atual = atual.prox

    p_indice = int(atual.base*s/p)
    num_paginas = int(atual.deslocamento*s/p)

    for i in range(p_indice, p_indice + num_paginas):
        mmu[i].processo = -1
        mmu[i].posicao = -1
        mmu[i].r = 0
        mmu[i].endereco_fisico = -1
