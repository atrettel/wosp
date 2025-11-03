# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 Andrew Trettel
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -Wfatal-errors -Werror -pedantic-errors -O2 -g
RM = rm
RMFLAGS = -frv
CP = cp
CPFLAGS = -nv

project = wosp
version = 0.0.0

DESTDIR = /opt/$(project)-$(version)/usr

OBJ = input.o interpreter.o operations.o output.o search.o words.o

$(project): $(project).c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	-$(RM) $(RMFLAGS) $(project)
	-$(RM) $(RMFLAGS) *.o
	-$(RM) $(RMFLAGS) $(project)-*.tar.gz

dist: clean
	mkdir $(project)-$(version)
	$(CP) $(CPFLAGS) Makefile README.md *.c *.h $(project).1 $(project)-$(version)
	tar -cvzf $(project)-$(version).tar.gz $(project)-$(version)
	-$(RM) $(RMFLAGS) $(project)-$(version)

install: $(project)
	mkdir -p $(DESTDIR)/bin
	cp -fv $(project) $(DESTDIR)/bin
	chmod 755 $(DESTDIR)/bin/$(project)
	mkdir -p $(DESTDIR)/share/man/man1
	cp -fv $(project).1 $(DESTDIR)/share/man/man1
	sed -i "s/VERSION/$(version)/g" $(DESTDIR)/share/man/man1/$(project).1
	chmod 644 $(DESTDIR)/share/man/man1/$(project).1

.PHONY: clean dist install
