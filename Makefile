#-------------------------------------------------
#  FILE: Makefile
#  AUTHOR:
#  EMAIL:
#  DATE:
#  BRIEF:
#  NOTE:
#-------------------------------------------------
CC=gcc
LINKS=-lm
EXEC=ifj
CFLAGS=-std=c11 -Wall -Wextra -pedantic -g
TEST_SRC=tests/src
TEST_BIN=tests/bin/
PARSER_C=src/main.c src/parser.c src/scanner.c src/dynamic_string.c src/indent_stack.c src/precedence_analysis.c src/general_stack.c



all:  test_scanner test_general_stack test_symtable

test_scanner: src/dynamic_string.c src/scanner.c src/indent_stack.c $(TEST_SRC)/test_scanner.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)



test_general_stack: src/general_stack.c $(TEST_SRC)/test_general_stack.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)



test_symtable: src/symtable.c $(TEST_SRC)/test_symtable.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)




tests: all
	./tests.sh

parser: $(PARSER_C) src/*
	gcc $(CFLAGS) -o $@ $(PARSER_C)


.PHONY: clean
clean:
	rm -f *.o ifj scanner_test parser
