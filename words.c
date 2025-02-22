/* Copyright (C) 2025 Andrew Trettel */
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

char *
reduce_word(char *original)
{
    size_t len = 0, j = 0;
    for (size_t i = 0; i < strlen(original); i++)
    {
        if (ispunct(original[i]) == false )
        {
            len++;
        }
    }
    len++;
    char *reduced = (char *) malloc(len * sizeof(char));
    for (size_t i = 0; i < strlen(original); i++)
    {
        if (ispunct(original[i]) == false )
        {
            reduced[j] = tolower(original[i]);
            j++;
        }
    }
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
    current->reduced = reduce_word(data);
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
sentence_ending_word(InputWord *word)
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

void
free_word(InputWord *list)
{
    if (list != NULL)
    {
        InputWord *prev = prev_word(list);
        free_word(prev);
        free(list->original);
        free(list->reduced);
        free(list);
    }
}
