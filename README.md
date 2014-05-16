Langmsg
===========

lmsg is a small tool for 
distributed node comunication.
It was designed for sending and receiving
messages within a cluster.

It's basically an interpreter that 
registers itself on a local or distributed name server
and provides variable assignments,message sending 
and message receiving, to allow you to
communicate with other nodes.

Up to now, it is only a shell for evaluating atoms,
message passing and pattern matching in the
erlang style.
The server and the distributed communication
will added soon.


### Syntax
    "@<node> ! <message>   --> send <message> to a distributed node"
    "@<node_name> ?        --> get last message sent to this node"

    "<object> = {}         --> creates a new local process"
    "<object> ! <message>  --> send <message> to <object>"
    "<object> ?            --> get last message sent to <object>"
    "<object> +            --> debug <object>"
    "<object>              --> get some info about <object>"



### Building up things
lmsg is written in pure ANSI C, so
it shall compile fine on unix/linux.
To build the shell run:

    cd src && make
    bin/lmsh  #run the shell
