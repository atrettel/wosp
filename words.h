/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2025 Andrew Trettel */
#ifndef WORDS_H
#define WORDS_H

#include <stdbool.h>

static const char wildcard_character = '?';
static const unsigned long end_field = 0;
static const unsigned long full_text_field = 1;

bool is_truncation_character(char);
bool is_clause_punctuation(char);
bool is_ending_punctuation(char);

typedef struct Word
{
    char *original;
    char *reduced; /* The lowercase word without any punctuation */
    char *filename;
    unsigned long line; /* Line and column for locating word in input */
    unsigned long column;
    unsigned long position; /* Position for order in doubly-linked list */
    unsigned long page;
    unsigned long field;
    struct Word *next;
    struct Word *prev;
} Word;

typedef enum LanguageElement
{
    LE_WORD,
    LE_CLAUSE,
    LE_LINE,
    LE_SENTENCE,
    LE_PARAGRAPH,
    LE_PAGE
} LanguageElement;

typedef enum WordOrigin
{
    WO_SOURCE,
    WO_QUERY
} WordOrigin;

typedef struct WordIterator
{
    Word *next;
    Word *(*direction_word)(Word *);
    bool limit_to_field;
    unsigned long prev_field;
} WordIterator;

char *reduce_word(char *, WordOrigin);
void append_word(Word **, char *, char *, unsigned long, unsigned long, unsigned long, unsigned long);
char *original_word(Word *);
char *reduced_word(Word *);
char *filename_word(Word *);
unsigned long line_word(Word *);
unsigned long column_word(Word *);
unsigned long position_word(Word *);
unsigned long page_word(Word *);
unsigned long field_word(Word *);
bool is_end_field(Word *);
bool field_has_next_word(Word *);
bool clause_ending_word(Word *);
bool sentence_ending_word(Word *);
bool paragraph_ending_word(Word *);
Word *next_boolean_element(Word *, bool boolean_word(Word *));
Word *prev_boolean_element(Word *, bool boolean_word(Word *));
Word *next_numbered_element(Word *, unsigned long element_word(Word *));
Word *prev_numbered_element(Word *, unsigned long element_word(Word *));
Word *next_word(Word *);
Word *prev_word(Word *);
Word *next_clause(Word *);
Word *prev_clause(Word *);
Word *next_line(Word *);
Word *prev_line(Word *);
Word *next_sentence(Word *);
Word *prev_sentence(Word *);
Word *next_paragraph(Word *);
Word *prev_paragraph(Word *);
Word *next_page(Word *);
Word *prev_page(Word *);
Word *list_first_word(Word *);
Word *list_last_word(Word *);
Word *field_first_word(Word *);
Word *field_last_word(Word *);
Word *document_word(Word *);
Word *advance_word(Word *, LanguageElement, int);
void print_words(Word *);
void free_words(Word *);

WordIterator init_word_iterator(Word *, Word *direction_word(Word *), bool);
Word *iterator_next_word(WordIterator *);
bool iterator_has_next_word(WordIterator);

#endif /* WORDS_H */
