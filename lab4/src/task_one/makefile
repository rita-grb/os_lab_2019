CC=gcc
CFLAGS=-I.

all: parallel_min_max

parallel_min_max: find_min_max.o parallel_min_max.c
	$(CC) parallel_min_max.c find_min_max.o utils.o -o parallel_min_max

find_min_max.o: utils.o find_min_max.c find_min_max.h
	$(CC) -c find_min_max.c -o find_min_max.o

utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o

clear:
	rm parallel_min_max find_min_max.o utils.o
