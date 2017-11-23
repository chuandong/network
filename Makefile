#####################################################################  
## file        : make all C file                                   ##  
## author      : chuandong                                         ##  
## time        : 07/11/2017                                        ##  
#####################################################################  
  
CC	= gcc
RM	= rm -rf
#CFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall
#CFLAGS = -g -O2 -Wall

PROGS=pollser pollcli

all: ${PROGS}

pollser:	pollser.o
	#${CC} ${CFLAGS} -o $@ pollser.o
	${CC} -o $@ pollser.c
pollcli:	pollcli.o
	${CC} ${CFLAGS} -o $@ pollser.o
	${CC} -o $@ pollser.c
clean:
	${RM} *.o
