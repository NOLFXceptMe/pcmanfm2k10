CC=gcc
CFLAGS=-Wall -g
CFLAGS+=`pkg-config --cflags glib-2.0`
LD=gcc
LDFLAGS+=`pkg-config --libs glib-2.0`

BIN=action
OBJS=parser.o

all: $(BIN)

main:

action: $(OBJS)

clean:
	rm -f $(OBJS) $(BIN)
