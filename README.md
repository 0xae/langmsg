Langmsg
===========

lmsg is a small tool for 
distributed node comunication.
It was designed for sending and receiving
messages within a cluster.

Is an interpreter that supports
variable assignments,message sending 
and message receiving.


Syntax
=============
    "   @<node> ! <message>  --> send <message> to a distributed node"
    "<node_name> ?            --> get last message sent to this node"

    "<object> ! <message>  --> send <message> to <object>"
    "<object> ?            --> get last message sent to <object>"
    "<object> +            --> debug <object>"
    "<object>              --> get some info about <object>"
    "<object> = {}         --> creates a new local process"


Building up things
=============
lmsg is written in pure ANSI C, so
it shall compile fine on unix/linux.
To build the shell run:

    cd src && make
    bin/lmsh  #run the shell
