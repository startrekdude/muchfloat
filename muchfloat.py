from collections import namedtuple
from multiprocessing import cpu_count, Manager, Process
from time import sleep, time

ThreadResult = namedtuple("ThreadResult", ("operation_count", "result"));

duration = 10.0
sample_size = 1000

def do_work(i, results):
	start = time()
	ops = 0
	acc = 0.1
	
	while time() - start < duration:
		for _ in range(sample_size):
			acc += 0.1
		ops += sample_size
	
	results[i] = ThreadResult(ops, acc)

def main():
	global results
	
	print("MuchFloat (Python) v0.9")
	cores = cpu_count()
	print("Number of processors: {}".format(cores))
	
	manager = Manager()
	results = manager.list([None for _ in range(cores)])
	
	jobs = []
	for i in range(cores):
		p = Process(target=do_work, args=(i, results))
		p.start()
		jobs.append(p)
	
	for p in jobs:
		p.join()
	
	sum = 0
	for i, result in enumerate(results):
		print("Thread {}: {} operations, result was {}".format(i+1, result.operation_count, result.result))
		sum += result.operation_count
	print("Total: {} operations".format(sum))

if __name__ == "__main__":
	main()