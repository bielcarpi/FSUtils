CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = fsutils

all: clean fsutils cleanObj

fsutils: fsutils.o ext2.o fat16.o tree.o
	$(CC) $(CFLAGS) -o fsutils fsutils.o ext2.o fat16.o tree.o

ext2.o: tree.o
	$(CC) $(CFLAGS) -c modules/ext2.c

fat16.o: tree.o
	$(CC) $(CFLAGS) -c modules/fat16.c

tree.o:
	$(CC) $(CFLAGS) -c modules/tree.c


clean:
	rm -f *.o $(TARGETS) *~

cleanObj:
	rm -f *.o
