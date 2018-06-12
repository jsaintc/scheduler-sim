sim: sim.c fcfs.c srtf.c hrrn.c rr.c
	gcc -o sim sim.c fcfs.c srtf.c hrrn.c rr.c -lm
