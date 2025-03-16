# Copyright (C) 2025 Andrew Trettel
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -Wfatal-errors -Werror -pedantic-errors -O2 -g
RM = rm
RMFLAGS = -frv

project = wosp

OBJ = interpreter.o operations.o search.o words.o

$(project): $(project).c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	-$(RM) $(RMFLAGS) $(project)
	-$(RM) $(RMFLAGS) *.o
