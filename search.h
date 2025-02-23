/* Copyright (C) 2025 Andrew Trettel */
#ifndef SEARCH_H
#define SEARCH_H

#include "words.h"

/* A continuous set of words matching a set of constraints */
typedef struct Match
{
    size_t n; /* Number of matched words */
    InputWord **words; /* Dynamically-allocated array of matching words */
    struct Match *next;
} Match;

void append_match(Match **, size_t);
void set_match(Match *, size_t, InputWord *);
size_t number_match(Match *);
unsigned int number_matches(Match *);
InputWord *word_match(Match *, size_t);
Match *next_match(Match *);
InputWord *start_match(Match *);
InputWord *end_match(Match *);
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
void free_trie(TrieNode *);

#endif /* SEARCH_H */
