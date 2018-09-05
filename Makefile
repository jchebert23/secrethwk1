CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
HWK    = /c/cs323/Hwk1

farthing: farthing.o
	${CC} ${CFLAGS} -o $@ $^ 

Farthing.o: farthing.c
