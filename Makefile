#Author: Ján Vavro
#Login: xvavro05

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c)) 
CC=gcc
NAME=ifj18
CFLAGS=-std=c99 -Wall -pedantic -Wextra -g
.PHONY: test

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

run:
	./$(NAME)

test:
	./test.sh

clean:
	rm -f $(NAME) *.o

