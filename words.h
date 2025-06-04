/* Copyright (C) 2025 Andrew Trettel */
#ifndef WORDS_H
#define WORDS_H

#include <stdbool.h>

bool is_ending_punctuation(char);

typedef struct Word
{
    char *original;
    char *reduced; /* The lowercase word without any punctuation */
    unsigned long line; /* Line and column for locating word in input */
    unsigned long column;
    unsigned long position; /* Position for order in doubly-linked list */
    struct Word *next;
    struct Word *prev;
} Word;

/* TODO: lines and paragraphs */
typedef enum LanguageElement
{
    LE_WORD,
    LE_CLAUSE,
    LE_SENTENCE
} LanguageElement;

typedef enum WordOrigin
{
    WO_SOURCE,
    WO_QUERY
} WordOrigin;

char *reduce_word(char *, WordOrigin);
void append_word(Word **, char *, unsigned long, unsigned long, unsigned long);
char *original_word(Word *);
char *reduced_word(Word *);
unsigned long line_word(Word *);
unsigned long column_word(Word *);
unsigned long position_word(Word *);
bool is_word(Word *);
bool has_next_word(Word *);
bool has_prev_word(Word *);
bool clause_ending_word(Word *);
bool sentence_ending_word(Word *);
Word *next_word(Word *);
Word *prev_word(Word *);
Word *next_clause(Word *);
Word *prev_clause(Word *);
Word *next_sentence(Word *);
Word *prev_sentence(Word *);
Word *first_word(Word *);
Word *last_word(Word *);
Word *advance_word(Word *, LanguageElement, int);
void print_words(Word *);
void free_words(Word *);

#endif /* WORDS_H */
