# Copyright (C) 2025 Andrew Trettel
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -Wfatal-errors -Werror -pedantic-errors -O2 -g
RM = rm
RMFLAGS = -frv
CP = cp
CPFLAGS = -nv

project = wosp

OBJ = interpreter.o operations.o output.o search.o words.o

$(project): $(project).c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	-$(RM) $(RMFLAGS) $(project)
	-$(RM) $(RMFLAGS) *.o
	-$(RM) $(RMFLAGS) $(project)-*.tar.gz

dist: clean
	mkdir $(project)-`date +"%F"`
	$(CP) $(CPFLAGS) Makefile README.md *.c *.h $(project)-`date +"%F"`
	tar -cvzf $(project)-`date +"%F"`.tar.gz $(project)-`date +"%F"`
	-$(RM) $(RMFLAGS) $(project)-`date +"%F"`

.PHONY: clean dist
