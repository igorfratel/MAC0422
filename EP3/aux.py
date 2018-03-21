#Allan Amancio Rocha - 9761614
#Igor Fratel Santana - 9793565

def imprime_memoria(bitmap, mmu, s, tempo):
	#Impressão do bitmap e do estado da memória
	print("-----------------------TEMPO: " + str(tempo) + " --------------------------------")
	print("Bitmap:")
	imprime_bitmap(bitmap, s)

	print('')

	print("\nMemória física:")
	with open("/tmp/ep3.mem", 'r') as f:
		for line in f: print(line, end = '')

	print('')

	print("Memória virtual:")
	with open("/tmp/ep3.vir", 'r') as f:
		for line in f: print(line, end = '')

	print('')

def imprime_bitmap(bitmap, s):
	atual = bitmap
	while(atual is not None):
		if(atual.estado == "P"):
			for i in range(0, int(atual.deslocamento * s)): print("1", end='')
		else:
			for i in range(0, int(atual.deslocamento * s)): print("0", end='')
		atual = atual.prox

def insere_tarefa(tarefas, x):
	#Insere tarefa ordenadamente por tempo
	tarefas.append(x)
	i = len(tarefas) - 2
	while(i >= 0 and x.tempo <= tarefas[i].tempo):
		if(x.tempo == tarefas[i].tempo):
			if(atribui_prioridade(x) <= atribui_prioridade(tarefas[i])): break
		tarefas[i+1], tarefas[i] = tarefas[i], tarefas[i+1]
		i -= 1

def atribui_prioridade(tarefa):
	#compacta -> cria -> acessa -> destroi
	if (tarefa.codigo == "compacta"): return 4
	if (tarefa.codigo == "cria"): return 3
	if (tarefa.codigo == "acessa"): return 2
	if (tarefa.codigo == "destroi"): return 1
