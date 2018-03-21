from random import uniform
import sys
import os

def trace_generator(num_processes, min_t0, max_t0, min_dt, max_dt, output_file):
	i = 0;
	num_processes = int(num_processes)
	min_t0 = float(min_t0)
	max_t0 = float(max_t0)
	min_dt = float(min_dt)
	max_dt = float(max_dt)
	with open("temp_trace_generator.txt", 'w') as f:
		while (i < num_processes):
			t0 = round(uniform(min_t0, max_t0), 1)
			dt = round(uniform(min_dt, max_dt), 1)
			deadline = round(uniform(t0 + dt, num_processes*(t0 + dt)), 1)
			f.write(str(t0) + " " + str(dt) + " " + str(deadline) + " processo" + str(i) + "\n")
			i = i + 1
	sort(output_file)

def sort(output_file):
	os.system("sort -g -k 1,1 temp_trace_generator.txt > " + output_file)

def main():
	#1: number of processes
	#2: min t0
	#3: max t0
	#4: min dt
	#5: max dt
	#7: output file
	trace_generator(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6])
main()