/* Copyright (C) 2025 Andrew Trettel */
#ifndef SEARCH_H
#define SEARCH_H

#include "words.h"

typedef struct Match
{
    size_t n; /* Number of matched words */
    InputWord **words;
    struct Match *next;
} Match;

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

#endif /* SEARCH_H */
