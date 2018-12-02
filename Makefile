#Author: Ján Vavro
#Login: xvavro05

.PHONY: test
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c)) 
CC=gcc
NAME=ifj18
CFLAGS=-std=c99 -Wall -pedantic -Wextra -g

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

run:
	./$(NAME)

test:
	./test.sh

clean:
	rm -f $(NAME) *.o

