/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2025 Andrew Trettel */
#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

#include "search.h"
#include "words.h"

void add_words_to_trie(TrieNode *, Word *);
void read_source_words(Word **, FILE *, char *);
size_t read_data(int, char **, TrieNode **, char ***, Word ***);
void free_data(size_t, TrieNode *, char **, Word **);

#endif /* INPUT_H */
