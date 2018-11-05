
NAME=main

DBG=kdbg
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra -g

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) lex.c parser.c main.c -lm -o $(NAME)

run:
	./$(NAME)

debug: $(NAME)
	$(DBG) $(NAME)

clean:
	rm -f *~ $(NAME) *.bak *.o

edit:
	vim $(NAME).c

