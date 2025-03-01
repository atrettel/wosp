# Copyright (C) 2025 Andrew Trettel
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -Wfatal-errors -Werror -pedantic-errors -O0 -g

project = wosp

OBJ = search.o words.o

$(project): $(project).c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	-rm -fv $(project)
	-rm -fv *.o
