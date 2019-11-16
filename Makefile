#-------------------------------------------------
#  FILE: Makefile
#  AUTHOR:
#  EMAIL:
#  DATE:
#  BRIEF:
#  NOTE:
#-------------------------------------------------
CC=gcc
EXEC=ifj
CFLAGS=-std=c11 -Wall -Wextra -pedantic -g
TEST_SRC=tests/src
TEST_BIN=tests/bin/

all:  test_scanner test_general_stack test_symtable

test_scanner: src/dynamic_string.c src/scanner.c src/indent_stack.c $(TEST_SRC)/test_scanner.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^



test_general_stack: src/general_stack.c $(TEST_SRC)/test_general_stack.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^



test_symtable: src/symtable.c $(TEST_SRC)/test_symtable.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^




tests: all
	rm output.txt ; touch output.txt ; ./tests.sh output.txt ; cat output.txt | grep "ERROR" ; cat output.txt | grep "usage"



.PHONY: clean
clean:
	rm -f *.o ifj scanner_test
