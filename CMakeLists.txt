cmake_minimum_required(VERSION 3.12)
project(IFJ C)

set(CMAKE_C_STANDARD 99)

add_executable(ifj18 main.c parser.c lex.c error.h lex.h parser.h)