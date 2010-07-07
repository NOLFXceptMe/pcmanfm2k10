CC=gcc
CFLAGS=-Wall -g
CFLAGS+=`pkg-config --cflags glib-2.0` `pkg-config --cflags libfm` -I$(INCLUDE) `pkg-config --cflags gio-unix-2.0`
LD=gcc
LDFLAGS+=`pkg-config --libs glib-2.0` `pkg-config --libs libfm` `pkg-config --libs gio-unix-2.0`

INCLUDE=include/
BIN=action profile showmenu
OBJS=parser.o validation.o parameter.o

all: $(BIN)

main:

action: parser.o

profile: parser.o

parametertest: parameter.o

validation.o: parser.o

showmenu: $(OBJS)

clean:
	rm -f $(OBJS) $(BIN)
