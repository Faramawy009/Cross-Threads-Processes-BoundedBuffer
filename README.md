# Threads Bounded Buffer
This program implements a bounded buffer across multiple threads in C using POSIX mechanisms of mutexes and condition variables in synchronization between "Color" producing and consuming processes.

## To run:

1. open terminal in the Threads Bounded Buffer directory

2. type in the command make

3. type in the command ./prod_cons


# Processes Bounded Buffer
This program aims to achieve the same bounded buffer as in the one above except that it handles that across processes so instead of having producer and consuming threads we have producing and consuming processes that communicate through shared memory mechanisms built in C.

## To run

1. open terminal in the Processes Bounded Buffer directory

2. type in the command make

3. type in the command ./main
