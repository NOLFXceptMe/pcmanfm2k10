CC=gcc
CFLAGS=-Wall -g
CFLAGS+=`pkg-config --cflags glib-2.0` `pkg-config --cflags libfm` -DNDEBUG
LD=gcc
LDFLAGS+=`pkg-config --libs glib-2.0` `pkg-config --libs libfm`

BIN=action profile showmenu
OBJS=parser.o

all: $(BIN)

main:

action: $(OBJS)

profile: $(OBJS)

showmenu: $(OBJS)

clean:
	rm -f $(OBJS) $(BIN)
