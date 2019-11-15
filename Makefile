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

all: scanner_test test_scanner test_general_stack test_symtable
	
scanner_test: src/scanner_test.c src/dynamic_string.c src/scanner.c src/indent_stack.c
	$(CC) $(CFLAGS) -o $@ src/scanner_test.c src/dynamic_string.c src/scanner.c src/indent_stack.c

test_scanner: src/scanner_test.c src/dynamic_string.c src/scanner.c src/indent_stack.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^



test_general_stack: src/general_stack.c $(TEST_SRC)/test_general_stack.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^



test_symtable: src/symtable.c $(TEST_SRC)/test_symtable.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^




tests: $(TEST_BIN)*
	for i in $(TEST_BIN)test*;do valgrind ./"$i" ;done



.PHONY: clean
clean:
	rm -f *.o ifj scanner_test
