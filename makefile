all:signal_jobs

signal_jobs:signal_jobs.c
	gcc -g signal_jobs.c -o signal_jobs
clean:
	rm signal_jobs
