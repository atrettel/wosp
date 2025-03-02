/* Copyright (C) 2025 Andrew Trettel */
#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>

#include "words.h"

/* A match is a continuous set of words matching a set of constraints.  Each
 * match is part of a linked list where subsequent matches are merely appended
 * onto the list. */
typedef struct Match
{
    size_t n; /* Number of searched words */
    InputWord **words; /* Dynamically-allocated array of searched words */
    struct Match *next;
} Match;

void append_match(Match **, size_t);
void set_match(Match *, size_t, InputWord *);
size_t number_of_words_in_match(Match *);
unsigned int length_of_match_list(Match *);
InputWord *word_match(Match *, size_t);
Match *next_match(Match *);
InputWord *start_word_match(Match *);
InputWord *end_word_match(Match *);
unsigned int start_position_match(Match *);
unsigned int end_position_match(Match *);
unsigned int width_match(Match *);
void print_matches(Match *);
Match *copy_matches(Match *);
void concatenate_matches(Match *, Match **);
void free_matches(Match *);

struct TrieNode;
struct TrieEdge;

typedef struct TrieNode
{
    char key;
    struct TrieEdge *edges;
    Match *match;
} TrieNode;

typedef struct TrieEdge
{
    struct TrieNode *node;
    struct TrieEdge *next;
} TrieEdge;

void init_trie(TrieNode **);
void insert_trie(TrieNode *, InputWord *, size_t);
Match *match_trie(TrieNode *, char *, size_t);
bool has_word_trie(TrieNode *, char *);
void backtrack_trie(TrieNode*, char *, size_t, Match **);
void free_trie(TrieNode *);

Match *word_search(TrieNode *, char *);
Match *wildcard_search(TrieNode *, char *);

#endif /* SEARCH_H */
