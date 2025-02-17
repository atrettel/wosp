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

struct TreeNode;
struct TreeEdge;

typedef struct TreeNode
{
    char key;
    struct TreeEdge *edges;
    Match *match;
} TreeNode;

typedef struct TreeEdge
{
    struct TreeNode *node;
    struct TreeEdge *next;
} TreeEdge;

#endif /* SEARCH_H */
