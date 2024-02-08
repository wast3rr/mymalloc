CC = gcc
CFLAGS = -Wall -std=c99

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o memgrind

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c -Wall mymalloc.c

memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c -Wall memgrind.c

clean:
	rm *.o memgrind
