CFLAGS= -Wall -g
CC = gcc

run: clean automate
	./automate 

automate: test.o automate.o
	$(CC) test.o automate.o -o automate

%.o: %.c automate.h
	$(CC) -c $< $(CFLAGS) 

valgrind: automate
	valgrind --leak-check=full ./automate

clean: 
	rm -f *.o
	rm -f automate
	rm -f *.aut