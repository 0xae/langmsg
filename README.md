langmsg
===========

lmsg is a small tool for 
distributed node comunication.
It was designed for sending and receiving
messages within a cluster.

Is an interpreter that supports
variable assignments,message sending 
and message receiving.


syntax
=============
    "   @node ! <message>  --> send <message> to a distributed node"
    "<node_name> ?            --> get last message sent to this node"

    "<object> ! <message>  --> send <message> to <object>"
    "<object> ?            --> get last message sent to <object>"
    "<object> +            --> debug <object>"
    "<object>              --> get some info about <object>"
    "<object> = {}         --> creates a new local process"


starting up
=============
The server is written in Java using
the popular netty.

 
