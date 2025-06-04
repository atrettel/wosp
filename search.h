/* Copyright (C) 2025 Andrew Trettel */
#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include <stddef.h>

#include "words.h"

typedef struct DocumentNode
{
    Word *document;
    struct DocumentNode *next;
} DocumentNode;

void insert_document(DocumentNode **, Word *document);
DocumentNode *next_document(DocumentNode *);
bool has_document(DocumentNode *, Word *);
bool is_document(DocumentNode *);
void free_document_list(DocumentNode *);

/* A match is a continuous set of words matching a set of constraints.  Each
 * match is part of a linked list where subsequent matches are merely appended
 * onto the list. */
typedef struct Match
{
    size_t n; /* Number of searched words */
    Word **words; /* Dynamically-allocated array of searched words */
    struct Match *next;
    Word *document;
} Match;

void insert_match(Match **, size_t);
void set_match(Match *, size_t, Word *);
void append_match(Match *, Match **);
size_t number_of_words_in_match(Match *);
unsigned int length_of_match_list(Match *);
Word *word_match(Match *, size_t);
Word *document_match(Match *);
DocumentNode *document_list_match_list(Match *);
bool is_match(Match *);
Match *next_match(Match *);
Word *start_word_match(Match *);
Word *end_word_match(Match *);
unsigned int start_position_match(Match *);
unsigned int end_position_match(Match *);
unsigned int width_match(Match *);
void print_matches(Match *);
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
void insert_trie(TrieNode *, Word *, size_t);
bool has_word_trie(TrieNode *, char *);
void backtrack_trie(TrieNode *, char *, size_t, Match **);
void expand_word(TrieNode *, char *, size_t, Match **);
void free_trie(TrieNode *);

Match *wildcard_search(TrieNode *, char *);
Match *proximity_search(Match *, Match *, LanguageElement, int, int);

#endif /* SEARCH_H */
