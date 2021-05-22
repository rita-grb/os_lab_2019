CC=gcc
CFLAGS=-I.

all: parallel_min_max sequential_min_max utils.o find_min_max.o EXECVL

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)


sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

execvl.o:
	$(CC) -c execvl.c -o execvl.o

EXECVL: execvl.o
	$(CC) execvl.o -o EXECVL

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
