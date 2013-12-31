dist: error_lib
	gcc -g -o ../bin/lclient error.o client.c -pthread
	gcc -g -o ../bin/lserver error.o server.c -pthread
	gcc -g -o tst error.o test.c -pthread	


lnet: error_lib
	gcc -o ../bin/lserver error.o lmsg_server.c
	gcc -o ../bin/lhost error.o lmsg_client.c


error_lib:
	gcc -c error.c

test:
	gcc -o ../bin/servtst tests.c

net:
	gcc -c network.c
