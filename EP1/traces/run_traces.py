import sys
import subprocess
def run_traces(scheduler, trace_prefix, output):
	for index in range(4, 30):
		subprocess.run(["./ep1t", scheduler, trace_prefix + str(index) + ".txt", output + str(index) + ".txt"])

def main():
	#1: scheduler
	#2: trace_files prefix
	#3: output file
	run_traces(sys.argv[1], sys.argv[2], sys.argv[3])
main()	
