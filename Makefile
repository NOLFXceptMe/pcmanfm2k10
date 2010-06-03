CC=gcc
CFLAGS=-Wall -g
CFLAGS+=`pkg-config --cflags glib-2.0`
LD=gcc
LDFLAGS+=`pkg-config --libs glib-2.0`

BIN=main
OBJS=parser.o

all: main

main: $(OBJS)

clean:
	rm -f $(OBJS) $(BIN)
