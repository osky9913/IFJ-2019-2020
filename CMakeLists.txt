cmake_minimum_required(VERSION 3.14)
project(project_ifj)

set(CMAKE_C_STANDARD 11)

add_executable(infToPost src/precedence_analysis.c src/precedence_analysis.h src/scanner.c src/scanner.h src/dynamic_string.c
        src/dynamic_string.h src/indent_stack.c src/indent_stack.h src/general_stack.c src/general_stack.h src/symtable.c src/symtable.h
        src/errors.h src/parser.c src/parser.h src/semantic.c src/semantic.h
        src/precedence_semantic.c src/precedence_semantic.h src/generate.c src/generate.h)