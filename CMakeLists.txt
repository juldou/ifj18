cmake_minimum_required(VERSION 3.8.2)
project(IFJ C)

set(CMAKE_C_STANDARD 99)

add_executable(ifj18
        main.c
        lex.h
        lex.c
        parser.c
        parser.h
        symtable.h
        symtable.c
        error.h
        expr_parser.c
        expr_parser.h
        stack.c
        stack.h
        error.h
        semantic.h
        semantic.c
        str.c
        str.h
        code_gen.c
        code_gen.h
        list.c
        list.h)