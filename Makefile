CC=gcc
CFLAGS=-Wall -g -O2
LD=gcc
LDFLAGS=

BIN=main
OBJS=parser.o

all: main

main: $(OBJS)

clean:
	rm -f $(OBJS) $(BIN)
