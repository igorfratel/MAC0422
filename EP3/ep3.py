#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

import sys #sys.argc e s.argv[]
from Tarefa import Tarefa
from simulacao import simulacao
from aux import insere_tarefa
import random as rand
from time import sleep

def carrega(arquivo):
    #Lida com as entradas do programa
    total = 0
    virtual = 0
    s = 0 #unidade de alocação
    p = 0 #tamanho da página
    tarefas = [] #Fila de objetos Tarefa contendo a tarefa a ser executada em cada momento.
    id = 0 #Contador para as ids dos processos adicionados

    #Lê a primeira linha de input
    with open(arquivo) as f:
        first_line = f.readline()
        first_line = first_line.split()
        total = int(first_line[0]) #o total de memória física que deve ser simulada
        virtual = int(first_line[1]) #o total de memória virtual que deve ser simulada
        s = int(first_line[2]) #é o tamanho da unidade de alocação a ser considerada para a execução dos algoritmos para gerência do espaço livre
        p = int(first_line[3]) #é o tamanho da página a ser considerada para a execução dos algoritmos de substituição de página
        #print("Primeira linha lida: " + str(total) + " " + str(virtual) + " " + str(s) + " " + str(p)) #!!! print temporario

        #Lê as demais linhas de input
        for line in f:
            line = line.split()
            if (line[1] == "COMPACTAR"):
                tarefa = Tarefa(int(line[0]), "compacta")
                insere_tarefa(tarefas, tarefa)

            else:
                tarefa = Tarefa(int(line[0]), "cria", id, int(line[2]), line[3])
                insere_tarefa(tarefas, tarefa)
                tarefa = Tarefa(int(line[1]), "destroi", id, line[3])
                insere_tarefa(tarefas, tarefa)
                for i in range(4, len(line) - 1, 2):
                    tarefa = Tarefa(int(line[i+1]), "acessa", id, int(line[i]))
                    insere_tarefa(tarefas, tarefa)
                id += 1

    return total, virtual, s, p, tarefas



def executa(intervalo, espaco, substitui, total, virtual, s, p, tarefas):
    if (espaco == 1): espaco = "best_fit"
    elif (espaco == 2): espaco = "worst_fit"
    elif (espaco == 3): espaco = "quick_fit"

    if (substitui == 1): substitui = "optimal"
    elif (substitui == 2): substitui = "fifo"
    elif (substitui == 3): substitui = "lru_2"
    elif (substitui == 4): substitui = "lru_4"

    simulacao(intervalo, espaco, substitui, total, virtual, s, p, tarefas)

def main():
    #mini shell
    while(1):
        command = input("[ep3]:")
        command = command.split()

        if (command[0] == "carrega"):
            if (len(command) != 2): print("Erro: Argumento inválido")
            else:
                arquivo = command[1]
                total, virtual, s, p, tarefas = carrega(arquivo)

        elif (command[0] == "espaco"):
            if (len(command) != 2): print("Erro: Argumento inválido")
            else:
                if (1 <= int(command[1]) <= 3): espaco = int(command[1])
                else: print("Erro: Algoritmo inválido")

        elif (command[0] == "substitui"):
            if (len(command) != 2): print("Erro: Argumento inválido")
            else:
                if (1 <= int(command[1]) <= 4): substitui = int(command[1])
                else: print("Erro: Algoritmo inválido")

        elif (command[0] == "executa"):
            if (len(command) != 2): print("Erro: Argumento inválido")
            else:
                if 'espaco' in locals() and 'substitui' in locals() and 'arquivo' in locals():
                    intervalo = int(command[1])
                    print("Executando arquivo " + arquivo + " com algoritmo de espaço " + str(espaco) + ", algoritmo de paginação " + str(substitui) + " e intervalo " + str(intervalo))
                    executa(intervalo, espaco, substitui, total, virtual, s, p, tarefas)
                else: print("Erro: Algoritmo(s) ou arquivo de leitura não definido(s)")

        elif (command[0] == "sai"): sys.exit(0)

        elif (command[0] == "aleatorio"):
            if (len(command) != 1): print("Erro: Argumento inválido")
            else:
                if 'arquivo' not in locals():
                    total, virtual, s, p, tarefas = carrega("trace.txt")
                intervalo = rand.randint(1, 18)
                espaco = rand.randint(1, 3)
                substitui = rand.randint(1, 4)
                print("Intervalo: " + str(intervalo) + "; Algoritmo de espaço: " + str(espaco) + "; Algoritmo de paginação: " + str(substitui))
                sleep(3)
                executa(intervalo, espaco, substitui, total, virtual, s, p, tarefas)

        elif (command[0] == "padrao"):
            if (len(command) != 1): print("Erro: Argumento inválido")
            else:
                if 'arquivo' not in locals():
                    total, virtual, s, p, tarefas = carrega("trace.txt")
                executa(1, 1, 1, total, virtual, s, p, tarefas)

        else: print("Erro: Comando inválido")

if __name__ == "__main__":
    main()
