CC=gcc
CFLAGS=-c -Wall -pthread

all: main

        main: main.o server.o client.o matrix.o
$(CC) main.o server.o client.o matrix.o -o main -pthread

        main.o: main.c
$(CC) $(CFLAGS) main.c

        server.o: server.c server.h
$(CC) $(CFLAGS) server.c

        client.o: client.c client.h
$(CC) $(CFLAGS) client.c

        matrix.o: matrix.c matrix.h
$(CC) $(CFLAGS) matrix.c
//g