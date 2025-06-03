/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "words.h"

bool
is_ending_punctuation(char c)
{
    if (c == '.' || c == '?' || c == '!')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
is_clause_punctuation(char c)
{
    if (c == ',' || c == ';' || c == ':' || is_ending_punctuation(c))
    {
        return true;
    }
    else
    {
        return false;
    }
}

char *
reduce_word(char *original, WordOrigin origin)
{
    size_t len = 0, j = 0;
    for (size_t i = 0; i < strlen(original); i++)
    {
        if ((ispunct(original[i]) == false) || ((original[i] == wildcard_character) && (origin == WO_QUERY)))
        {
            len++;
        }
    }
    len++;
    char *reduced = (char *) malloc(len * sizeof(char));
    if (reduced == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < strlen(original); i++)
    {
        if ((ispunct(original[i]) == false) || ((original[i] == wildcard_character) && (origin == WO_QUERY)))
        {
            if (case_sensitive == true)
            {
                reduced[j] = original[i];
            }
            else
            {
                reduced[j] = tolower(original[i]);
            }
            j++;
        }
    }
    reduced[len-1] = '\0';
    return reduced;
}

void
append_word(Word **list, char *data, unsigned long line,
            unsigned long column, unsigned long position)
{
    Word *current = (Word *) malloc(sizeof(Word));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->original = data;
    current->reduced = reduce_word(data, WO_SOURCE);
    current->line = line;
    current->column = column;
    current->position = position;
    current->next = NULL;
    current->prev = *list;
    if (has_word(*list) == true)
    {
        (*list)->next = current;
    }
    *list = current;
}

char *
original_word(Word *word)
{
    return word->original;
}

char *
reduced_word(Word *word)
{
    return word->reduced;
}

unsigned long
line_word(Word *word)
{
    return word->line;
}

unsigned long
column_word(Word *word)
{
    return word->column;
}

unsigned long
position_word(Word *word)
{
    return word->position;
}

bool
has_word(Word *word)
{
    if (word == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
has_next_word(Word *word)
{
    if ((has_word(          word)  == false) ||
        (has_word(next_word(word)) == false))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
has_prev_word(Word *word)
{
    if ((has_word(          word)  == false) ||
        (has_word(prev_word(word)) == false))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
clause_ending_word(Word *word)
{
    if (has_word(word) == true)
    {
        if (sentence_ending_word(word) == true)
        {
            return true;
        }
        else
        {
            char *data = original_word(word);
            size_t len = strlen(data);
            return is_clause_punctuation(data[len-1]);
        }
    }
    else
    {
        return true;
    }
}

bool
sentence_ending_word(Word *word)
{
    if (has_word(word) == true)
    {
        char *data = original_word(word);
        size_t len = strlen(data);
        if (has_next_word(word) == false)
        {
            return is_ending_punctuation(data[len-1]);
        }
        else
        {
            char *next_data = original_word(next_word(word));
            if ((is_ending_punctuation(data[len-1]) == true) && (isupper(next_data[0])))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        return true;
    }
}

Word *
next_word(Word *word)
{
    if (has_word(word) == true)
    {
        return word->next;
    }
    else
    {
        return NULL;
    }
}

Word *
prev_word(Word *word)
{
    if (has_word(word) == true)
    {
        return word->prev;
    }
    else
    {
        return NULL;
    }
}

Word *
next_clause(Word *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        Word *current = word;
        while (clause_ending_word(current) == false)
        {
            current = next_word(current);
        }
        return next_word(current);
    }
}

Word *
prev_clause(Word *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        Word *current = word;
        if (clause_ending_word(current) == true)
        {
            current = prev_word(current);
        }
        while (clause_ending_word(current) == false)
        {
            Word *prev = prev_word(current);
            if (prev == NULL)
            {
                return current;
            }
            current = prev;
        }
        return next_word(current);
    }
}

Word *
next_sentence(Word *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        Word *current = word;
        while (sentence_ending_word(current) == false)
        {
            current = next_word(current);
        }
        return next_word(current);
    }
}

Word *
prev_sentence(Word *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        Word *current = word;
        if (sentence_ending_word(current) == true)
        {
            current = prev_word(current);
        }
        while (sentence_ending_word(current) == false)
        {
            Word *prev = prev_word(current);
            if (prev == NULL)
            {
                return current;
            }
            current = prev;
        }
        return next_word(current);
    }
}

Word *
first_word(Word *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        Word *current = word;
        while (has_prev_word(current) == true)
        {
            current = prev_word(current);
        }
        return current;
    }
}

Word *
last_word(Word *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        Word *current = word;
        while (has_next_word(current) == true)
        {
            current = next_word(current);
        }
        return current;
    }
}

/* This is a wrapper function to handle the ends of the input word list safely.
 * The primitive operations can return NULL values.  This is necessary to check
 * for the ends of the list.  This function cannot return NULL values. */
Word *
advance_word(Word *word, LanguageElement element, int n)
{
    Word *current = word;
    if (n != 0)
    {
        Word *(*advance)(Word *) = NULL;
        if (element == LE_WORD && n > 0)
        {
            advance = next_word;
        }
        else if (element == LE_WORD && n < 0)
        {
            advance = prev_word;
        }
        else if (element == LE_CLAUSE && n > 0)
        {
            advance = next_clause;
        }
        else if (element == LE_CLAUSE && n < 0)
        {
            advance = prev_clause;
        }
        else if (element == LE_SENTENCE && n > 0)
        {
            advance = next_sentence;
        }
        else if (element == LE_SENTENCE && n < 0)
        {
            advance = prev_sentence;
        }
        assert(advance != NULL);

        size_t m = abs(n);
        current = word;
        for (size_t i = 0; i < m; i++)
        {
            Word *next = advance(current);
            if (next != NULL)
            {
                current = next;
            }
            else
            {
                if (n > 0)
                {
                    current = last_word(current);
                }
                else
                {
                    current = first_word(current);
                }
            }
        }
    }
    return current;
}

void
print_words(Word *list)
{
    Word *current = list;
    while (has_word(current) == true)
    {
        printf("%10zu: '%s' ('%s')", position_word(current), original_word(current), reduced_word(current));
        if (sentence_ending_word(current) == true)
        {
            printf(" ...");
        }
        printf("\n");
        current = next_word(current);
    }
}

void
free_words(Word *list)
{
    Word *current = list;
    while (has_word(current) == true)
    {
        Word *next = next_word(current);
        free(current->original);
        free(current->reduced);
        free(current);
        current = next;
    }
}
