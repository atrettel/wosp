/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdlib.h>

#include "search.h"

void
append_match(Match **list, size_t n)
{
    Match *current = (Match *) malloc(sizeof(Match));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->n = n;
    current->words = (InputWord **) malloc(n * sizeof(InputWord *));
    if (current->words == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; i++)
    {
        current->words[i] = NULL;
    }
    current->next = *list;
    *list = current;
}

void
set_match(Match *match, size_t i, InputWord *word)
{
    assert(i >= 0);
    assert(i < number_match(match));
    match->words[i] = word;
}

size_t
number_match(Match *match)
{
    return match->n;
}

InputWord *
word_match(Match *match, size_t i)
{
    assert(i >= 0);
    assert(i < number_match(match));
    return match->words[i];
}

Match *
next_match(Match *match)
{
    if (match == NULL)
    {
        return NULL;
    }
    else
    {
        return match->next;
    }
}

InputWord *
start_match(Match *match)
{
    InputWord *start = word_match(match, 0);
    size_t n = number_match(match);
    for (size_t i = 1; i < n; i++)
    {
        InputWord *word = word_match(match, i);
        if (position_word(word) < position_word(start))
        {
            start = word;
        }
    }
    return start;
}

InputWord *
end_match(Match *match)
{
    InputWord *end = word_match(match, 0);
    size_t n = number_match(match);
    for (size_t i = 1; i < n; i++)
    {
        InputWord *word = word_match(match, i);
        if (position_word(word) > position_word(end))
        {
            end = word;
        }
    }
    return end;
}

void
free_matches(Match *list)
{
    Match *current = list;
    while (current != NULL)
    {
        Match *next = next_match(current);
        free(current->words);
        free(current);
        current = next;
    }
}
