CC = gcc
CFLAGS = -Wall -std=c99

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o memgrind

memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o memtest

testclient: testclient.o mymalloc.o
	$(CC) $(CFlAGS) $^ -o testclient

memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c -Wall memtest.c

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c -Wall mymalloc.c

memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c -Wall memgrind.c

clean:
	rm -f *.o memgrind memtest testclient
