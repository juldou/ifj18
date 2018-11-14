cmake_minimum_required(VERSION 3.8.2)
project(IFJ C)

set(CMAKE_C_STANDARD 99)

add_executable(ifj18
        main.c
        lex.h
        lex.c
        parser.h
        parser.c
        symtable.h
        symtable.c
        error.h
        semantic.h
        semantic.c)