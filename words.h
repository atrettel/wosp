/* Copyright (C) 2025 Andrew Trettel */
#ifndef WORDS_H
#define WORDS_H

#include <stdbool.h>

bool is_ending_punctuation(char);

typedef struct InputWord
{
    char *original;
    char *reduced; /* The lowercase word without any punctuation */
    unsigned long line; /* Line and column for locating word in input */
    unsigned long column;
    unsigned long position; /* Position for order in doubly-linked list */
    struct InputWord *next;
    struct InputWord *prev;
} InputWord;

typedef enum LanguageElement
{
    WORD,
    SENTENCE
} LanguageElement;

typedef enum WordOrigin
{
    SOURCE,
    QUERY
} WordOrigin;

char *reduce_word(char *, WordOrigin);
void append_word(InputWord **, char *, unsigned long, unsigned long, unsigned long);
char *original_word(InputWord *);
char *reduced_word(InputWord *);
unsigned long line_word(InputWord *);
unsigned long column_word(InputWord *);
unsigned long position_word(InputWord *);
bool sentence_ending_word(InputWord *);
InputWord *next_word(InputWord *);
InputWord *prev_word(InputWord *);
InputWord *next_sentence(InputWord *);
InputWord *prev_sentence(InputWord *);
InputWord *first_word(InputWord *);
InputWord *last_word(InputWord *);
InputWord *advance_word(InputWord *, LanguageElement, int);
void print_words(InputWord *);
void free_words(InputWord *);

#endif /* WORDS_H */
