#-------------------------------------------------
#  FILE: Makefile
#  AUTHOR:
#  EMAIL:
#  DATE:
#  BRIEF:
#  NOTE:
#-------------------------------------------------

EXEC=ifj
CFLAGS=-std=c11 -Wall -Wextra -pedantic -g

all: scanner_test
	
scanner_test: src/scanner_test.c src/dynamic_string.c src/scanner.c src/indent_stack.c
	gcc $(CFLAGS) -o $@ src/scanner_test.c src/dynamic_string.c src/scanner.c src/indent_stack.c



.PHONY: clean
clean:
	rm -f *.o ifj scanner_test
