DEBUG=-g -DDEBUG -Wall
OPTIMUS=-O2
BIN=../bin
TESTS=../tests
CC=gcc

all: lib code
lib: anet ae logging list hashtable
code: object testing shell

# *----------------- Library -------------------*
#

anet:
	$(CC) $(DEBUG) $(OPTS) -c anet.c   

logging:
	$(CC) $(DEBUG) $(OPTS) -c logging.c

ae:
	$(CC) $(DEBUG) $(OPTS) -c ae.c

list:
	$(CC) $(DEBUG) -c lmlist.c

hashtable:
	$(CC) $(DEBUG) -c lmtable.c



# *----------------- Real code -------------------*

object:
	$(CC) $(DEBUG) -c lmobject.c

eval:
	$(CC) $(DEBUG) -c lmeval.c

lmsgi:
	$(CC) $(DEBUG) -c lmsgi.c

lmsgi_main: object eval list hashtable
	$(CC) $(DEBUG) -o $(BIN)/lmsgi lmobject.o lmeval.o lmlist.o lmtable.o lmsgi.o lmsgi_main.c -pthread

testing:
	$(CC) $(DEBUG) $(OPTS) -o $(BIN)/testing lmobject.o lmlist.o testing.c
shell:
	$(CC) $(DEBUG) -o $(BIN)/lmsh lmobject.o lmlist.o lmtable.o lmshell.c -pthread

