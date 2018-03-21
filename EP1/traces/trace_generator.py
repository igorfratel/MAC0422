from random import uniform
import sys
import os

def trace_generator(num_processes, min_t0, max_t0, min_dt, max_dt, output_file, num_files):
	num_processes = int(num_processes)
	min_t0 = float(min_t0)
	max_t0 = float(max_t0)
	min_dt = float(min_dt)
	max_dt = float(max_dt)
	num_files = int(num_files)
	for file_index in range(0, num_files):
		with open("temp_trace_generator.txt", 'w') as f:
			i = 0;
			while (i < num_processes):
				t0 = round(uniform(min_t0, max_t0), 1)
				dt = round(uniform(min_dt, max_dt), 1)
				deadline = round(uniform(t0 + dt, t0 + 3*dt), 1)
				f.write(str(t0) + " " + str(dt) + " " + str(deadline) + " processo" + str(i) + "\n")
				i = i + 1
		output_file_temp = output_file + str(file_index) + ".txt"
		sort(output_file_temp)

def sort(output_file):
	os.system("sort -g -k 1,1 temp_trace_generator.txt > " + output_file)

def main():
	#1: number of processes
	#2: min t0
	#3: max t0
	#4: min dt
	#5: max dt
	#7: output file
	#8: how many files
	if (sys.argv[1] == "muito"):
		trace_generator(50, 0, 10, 0.1, 10, sys.argv[2], 30) #output_file
	elif (sys.argv[1] == "medio"):
		trace_generator(20, 0, 15, 0.1, 15, sys.argv[2], 30)
	elif (sys.argv[1] == "pouco"):
		trace_generator(4, 0, 20, 0.1, 20, sys.argv[2], 30)
	else:
		trace_generator(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], sys.argv[7])
main()