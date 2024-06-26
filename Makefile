CC = gcc

all: programa.o cpu.o disk.o ram.o

programa.o: programa.c 
	$(CC) programa.c -o programa -Wall

cpu.o: cpu.c
	$(CC) cpu.c -o cpu -Wall

disk.o: disk.c
	sudo $(CC) disk.c -o disco -Wall

ram.o: ram.c
	$(CC) ram.c -o ram -Wall
