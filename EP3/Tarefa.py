#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

class Tarefa:

    def __init__(self, *args):
        #Método construtor
        self.tempo = args[0]
        self.codigo = args[1] #"acessa" ou "compacta"
        if (self.codigo == "acessa"):
            self.id = args[2] #identificador do processo que vai acessar a posicao
            self.posicao = args[3] #posicao de memoria a ser acessada

        elif (self.codigo == "cria"):
            self.id = args[2]
            self.b = args[3] #tamanho do processo
            self.nome = args[4]

        elif (self.codigo == "destroi"):
            self.id = args[2]
            self.nome = args[3]
