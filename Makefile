# Rules for building each .o
cacheSimulator.exe: main.c
	gcc -g -o cacheSimulator.exe main.c -lm
