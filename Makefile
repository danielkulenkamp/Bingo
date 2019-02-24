
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC=gcc
endif
ifeq ($(UNAME_S),Darwin)
	CC=gcc-8
endif

CFLAGS=--std=c99 -Wall -Werror

all: p2p

main: main.o bingo.o random_bag.o
	$(CC) $(CFLAGS) -o main main.o bingo.o random_bag.o -lpthread

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

bingo: bingo.o
	$(CC) $(CFLAGS) -o bingo bingo.o random_bag.o

bingo.o: bingo.c bingo.h random_bag.h
	$(CC) $(CFLAGS) -c bingo.c

random_bag.o: random_bag.c random_bag.h
	$(CC) $(CFLAGS) -c random_bag.c

p2p: manager peer

peer: peer.o bingo.o random_bag.o
	$(CC) $(CFLAGS) -o peer peer.o bingo.o random_bag.o -lpthread

manager: manager.o
	$(CC) $(CFLAGS) -o manager manager.o -lpthread

peer.o: peer.c p2p.h peer.h
	$(CC) $(CFLAGS) -c peer.c

manager.o: manager.c p2p.h manager.h
	$(CC) $(CFLAGS) -c manager.c

cleanp2p:
	rm  peer manager peer.o manager.o

cleanbingo:
	rm main bingo.o random_bag.o main.o

cleanAll:
	rm main peer manager bingo *.o *.gch