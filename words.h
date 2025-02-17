/* Copyright (C) 2025 Andrew Trettel */
#ifndef WORDS_H
#define WORDS_H
#include <ctype.h>
#include <stdbool.h>

bool is_line_break(char);
bool is_whitespace(char);
bool is_ending_punctuation(char);
bool is_punctuation(char);

typedef struct InputWord
{
    char *original;
    char *reduced; /* The lowercase word without any punctuation */
    unsigned int line; /* Line and column for locating word in input */
    unsigned int column;
    unsigned int position; /* Position for order in doubly-linked list */
    bool ends_sentence;
    struct InputWord *next;
    struct InputWord *prev;
} InputWord;

char *reduce_word(char *);
void append_word(InputWord **, char *, unsigned int, unsigned int, unsigned int);
char *original_word(InputWord *);
char *reduced_word(InputWord *);
unsigned int line_word(InputWord *);
unsigned int column_word(InputWord *);
unsigned int position_word(InputWord *);
bool sentence_ending_word(InputWord *);
InputWord *next_word(InputWord *);
InputWord *prev_word(InputWord *);
void free_word(InputWord *);

#endif /* WORDS_H */
