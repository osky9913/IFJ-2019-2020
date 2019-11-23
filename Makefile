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
PARSER_C=src/main.c src/parser.c src/scanner.c src/dynamic_string.c src/indent_stack.c src/precedence_analysis.c src/general_stack.c src/symtable.c src/infixToPostfix.c src/semantic.c



all:  test_scanner test_general_stack test_symtable test_scanner_id

test_scanner: src/dynamic_string.c src/scanner.c src/general_stack.c $(TEST_SRC)/test_scanner.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)



test_general_stack: src/general_stack.c $(TEST_SRC)/test_general_stack.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)



test_symtable: src/symtable.c $(TEST_SRC)/test_symtable.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)

test_scanner_id: src/dynamic_string.c src/scanner.c src/general_stack.c $(TEST_SRC)/test_scanner_id.c
	$(CC) $(CFLAGS) -o $(TEST_BIN)$@ $^ $(LINKS)


tests_random: all
	rm output.txt ; touch output.txt ; ./tests.sh output.txt ; cat output.txt | grep "ERROR" ; cat output.txt | grep "usage"

parser: $(PARSER_C) src/*
	gcc $(CFLAGS) -o $@ $(PARSER_C)


tests_scanner_test_case: test_scanner_id
	cd tests/src/; \
	./test1.sh ; ./test2.sh ; ./test3.sh ; \
	./test4.sh ; ./test5.sh ; ./test6.sh ; \
	./test7.sh ; ./test8.sh ; ./test9.sh ; \
    ./test10.sh ; ./test11.sh ; ./test12.sh ; \
    ./test13.sh


infoToPost: $(PARSER_C) src/* 
	$(CC) $(CFLAGS) -o $@ $(PARSER_C) 



.PHONY: clean
clean:
	rm -f *.o ifj scanner_test parser
