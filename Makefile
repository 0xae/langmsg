BIN=bin/
TESTS=tests/
DEBUG_OPTS=-DDEBUG -g

lmsgi:list hashtable lmsg
	gcc -o $(BIN)lmsgi lmlist.o lmhashtable.o lmmsg.o main.c -lreadline
lmsgi_test: list hashtable lmsg
	gcc $(DEBUG_OPTS) -o $(TESTS)lmsgi_test lmlist.o lmhashtable.o lmmsg.o main.c -lreadline


lmsg:
	gcc -c lmmsg.c	


list:
	gcc -c lmlist.c
list_test: list
	gcc $(DEBUG_OPTS) -o $(TESTS)list_test lmlist.o lmlist_test.c


hashtable:
	gcc -c lmhashtable.c
hashtable_test: hashtable
	gcc $(DEBUG_OPTS) -o $(TESTS)hashtable_test lmhashtable.o lmhashtable_test.c


list_threaded:
	
hashtable_threaded:
