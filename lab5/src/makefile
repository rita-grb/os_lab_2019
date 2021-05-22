CC=gcc 
-std=c99

all: mutex fact_mod deadlock_example

mutex: mutex.c
	$(CC) mutex.c -lpthread -o mutex

fact_mod: fact_mod.c
	$(CC) fact_mod.c -lpthread -o fact_mod

deadlock_example: deadlock_example.c
	$(CC) deadlock_example.c -lpthread -o deadlock_example

clear:
	rm mutex fact_mod deadlock_example
