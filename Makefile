DEBUG=-g -DDEBUG -Wall
OPTIMUS=-O2
BIN=bin
TESTS=tests
CC=gcc
PARAMS=-Isrc/

all: lang shell

lang:
	$(CC) $(DEBUG) $(PARAMS) -c src/lang/lmeval.c
	$(CC) $(DEBUG) $(PARAMS) -c src/lang/lmlist.c
	$(CC) $(DEBUG) $(PARAMS) -c src/lang/lmobject.c
	$(CC) $(DEBUG) $(PARAMS) -c src/lang/lmtable.c
	$(CC) $(DEBUG) $(PARAMS) -c src/lang/logging.c

networking:
	$(CC) $(DEBUG) -c src/networking/ae.c
	$(CC) $(DEBUG) -c src/networking/ae_select.c
	$(CC) $(DEBUG) -c src/networking/anet.c

utils:
	$(CC) $(DEBUG) -c src/utils/zmalloc.c

shell:
	$(CC) $(DEBUG) $(PARAMS) -o $(BIN)/lmsh lmobject.o lmlist.o lmtable.o lmshell.c -pthread

