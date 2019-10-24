#-------------------------------------------------
#  FILE: Makefile
#  AUTHOR:
#  EMAIL:
#  DATE:
#  BRIEF:
#  NOTE:
#-------------------------------------------------

EXEC= ifj
CFLAGS= -std=c11 -Wall -Wextra -pedantic -g

all: string_test
	
string_test: src/string_test.c src/dynamic_string.c
	gcc $(CFLAGS) -o $@ src/string_test.c src/dynamic_string.c

.PHONY: clean
clean:
	rm -f *.o ifj string_test
