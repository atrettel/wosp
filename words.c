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
        if ((ispunct(original[i]) == false) || ((original[i] == wildcard_character) && (origin == QUERY)))
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
        if ((ispunct(original[i]) == false) || ((original[i] == wildcard_character) && (origin == QUERY)))
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
append_word(InputWord **list, char *data, unsigned long line,
            unsigned long column, unsigned long position)
{
    InputWord *current = (InputWord *) malloc(sizeof(InputWord));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->original = data;
    current->reduced = reduce_word(data, SOURCE);
    current->line = line;
    current->column = column;
    current->position = position;
    current->next = NULL;
    current->prev = *list;
    if ((*list) != NULL)
    {
        (*list)->next = current;
    }
    *list = current;
}

char *
original_word(InputWord *word)
{
    return word->original;
}

char *
reduced_word(InputWord *word)
{
    return word->reduced;
}

unsigned long
line_word(InputWord *word)
{
    return word->line;
}

unsigned long
column_word(InputWord *word)
{
    return word->column;
}

unsigned long
position_word(InputWord *word)
{
    return word->position;
}

bool
clause_ending_word(InputWord *word)
{
    if (word == NULL)
    {
        return true;
    }
    else
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
}

bool
sentence_ending_word(InputWord *word)
{
    if (word == NULL)
    {
        return true;
    }
    else
    {
        char *data = original_word(word);
        size_t len = strlen(data);
        InputWord *next = next_word(word);
        if (next == NULL)
        {
            return is_ending_punctuation(data[len-1]);
        }
        else
        {
            char *next_data = original_word(next);
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
}

InputWord *
next_word(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        return word->next;
    }
}

InputWord *
prev_word(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        return word->prev;
    }
}

InputWord *
next_clause(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        InputWord *current = word;
        while (clause_ending_word(current) == false)
        {
            current = next_word(current);
        }
        return next_word(current);
    }
}

InputWord *
prev_clause(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        InputWord *current = word;
        if (clause_ending_word(current) == true)
        {
            current = prev_word(current);
        }
        while (clause_ending_word(current) == false)
        {
            InputWord *prev = prev_word(current);
            if (prev == NULL)
            {
                return current;
            }
            current = prev;
        }
        return next_word(current);
    }
}

InputWord *
next_sentence(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        InputWord *current = word;
        while (sentence_ending_word(current) == false)
        {
            current = next_word(current);
        }
        return next_word(current);
    }
}

InputWord *
prev_sentence(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        InputWord *current = word;
        if (sentence_ending_word(current) == true)
        {
            current = prev_word(current);
        }
        while (sentence_ending_word(current) == false)
        {
            InputWord *prev = prev_word(current);
            if (prev == NULL)
            {
                return current;
            }
            current = prev;
        }
        return next_word(current);
    }
}

InputWord *
first_word(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        InputWord *current = word;
        while (prev_word(current) != NULL)
        {
            current = prev_word(current);
        }
        return current;
    }
}

InputWord *
last_word(InputWord *word)
{
    if (word == NULL)
    {
        return NULL;
    }
    else
    {
        InputWord *current = word;
        while (next_word(current) != NULL)
        {
            current = next_word(current);
        }
        return current;
    }
}

/* This is a wrapper function to handle the ends of the input word list safely.
 * The primitive operations can return NULL values.  This is necessary to check
 * for the ends of the list.  This function cannot return NULL values. */
InputWord *
advance_word(InputWord *word, LanguageElement element, int n)
{
    InputWord *current = word;
    if (n != 0)
    {
        InputWord *(*advance)(InputWord *) = NULL;
        if (element == WORD && n > 0)
        {
            advance = next_word;
        }
        else if (element == WORD && n < 0)
        {
            advance = prev_word;
        }
        else if (element == CLAUSE && n > 0)
        {
            advance = next_clause;
        }
        else if (element == CLAUSE && n < 0)
        {
            advance = prev_clause;
        }
        else if (element == SENTENCE && n > 0)
        {
            advance = next_sentence;
        }
        else if (element == SENTENCE && n < 0)
        {
            advance = prev_sentence;
        }
        assert(advance != NULL);

        size_t m = abs(n);
        current = word;
        for (size_t i = 0; i < m; i++)
        {
            InputWord *next = advance(current);
            if (next != NULL)
            {
                current = next;
            }
        }
    }
    return current;
}

void
print_words(InputWord *list)
{
    InputWord *current = list;
    while (current != NULL)
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
free_words(InputWord *list)
{
    InputWord *current = list;
    while (current != NULL)
    {
        InputWord *next = next_word(current);
        free(current->original);
        free(current->reduced);
        free(current);
        current = next;
    }
}
