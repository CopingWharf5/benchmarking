CC=gcc
CFLAGS=-c -Wall

all : client.o server.o
	gcc -o client.o client.c
	gcc -o server.o server.c -pthread

client.o : client.c
	gcc -c client.c

server.o : server.c
	gcc -c server.c
