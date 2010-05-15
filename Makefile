CC=gcc
CFLAGS=-Wall -g -O2
LD=gcc
LDFLAGS=

parser:

all: parser

clean:
	rm -f $(BINDIR)/*
