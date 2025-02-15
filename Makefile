# Copyright (C) 2025 Andrew Trettel
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wfatal-errors -Werror -pedantic-errors -O2

project=wrdh

$(project): $(project).c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	-rm -fv $(project)
	-rm -fv *.o
