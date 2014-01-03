langmsg
=====

lmsg is a small tool for 
distributed node comunication.
It was designed for sending and receiving
messages within a cluster.

These tool is an interpreter that supports
variable assignments, functions literals,
message sending and message receiving.

syntax
============
There are basically three types of objects
* funs(cool functions) Not suported yet
* objects(those that sends and receive messages)
* strings(those raw strings )

Assignment
----------------------
var = proc | fun{ instructions }

Message Passsing
-----------------------
var ! message
var ! { code here }

Message Receiving
-----------------------
var ?

