#-------------------------------------------------
#  FILE: Makefile
#  AUTHOR:
#  EMAIL:
#  DATE:
#  BRIEF:
#  NOTE:
#-------------------------------------------------

PROJECT=IFJ
CFLAGS= -std=c11 -Wall -Wextra -pedantic -g
CC=gcc

all:
	$(CC) $(CFLAGS) -o $(PROJECT) src/main.c src/scanner.c src/dynamic_string.c

.PHONY: clean
clean:
	rm -f *.o
