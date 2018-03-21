#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

class Node:
    #nó da lista ligada do bitmap
    def __init__(self, estado, processo, tamanho_processo, base, deslocamento):
        self.estado = estado
        self.processo = processo
        self.tamanho_processo = tamanho_processo #Processo alocado
        self.base = base
        self.deslocamento = deslocamento #Página alocada
        self.prox = None
