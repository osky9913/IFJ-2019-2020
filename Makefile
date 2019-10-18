#-------------------------------------------------
#  FILE: Makefile
#  AUTHOR:
#  EMAIL:
#  DATE:
#  BRIEF:
#  NOTE:
#-------------------------------------------------


PROJECT=IFJ
CC=gcc
CFLAGS= -std=c11 -Wall -Wextra -pedantic -g

all: $(PROJECT)
	

$(PROJECT) :  src/main.c src/dynamic_string.c
	$(CC) $(CFLAGS)  -o $@ src/main.c src/dynamic_string.c


.PHONY: clean
clean:
	rm -f *.o
