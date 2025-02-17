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
    bool ends_sentence;
    struct InputWord *next;
    struct InputWord *prev;
} InputWord;

char *reduce_word(char *);
void append_word(InputWord **, char *, unsigned long, unsigned long, unsigned long);
char *original_word(InputWord *);
char *reduced_word(InputWord *);
unsigned long line_word(InputWord *);
unsigned long column_word(InputWord *);
unsigned long position_word(InputWord *);
bool sentence_ending_word(InputWord *);
InputWord *next_word(InputWord *);
InputWord *prev_word(InputWord *);
void free_word(InputWord *);

#endif /* WORDS_H */
