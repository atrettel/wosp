/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "words.h"

bool
is_extended_wildcard_character(char c)
{
    if (c == '$' || c == '#')
    {
        return true;
    }
    else
    {
        return false;
    }
}

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
            reduced[j] = original[i];
            j++;
        }
    }
    reduced[len-1] = '\0';
    return reduced;
}

void
append_word(Word **list, char *data, char *filename, unsigned long line,
            unsigned long column, unsigned long position, unsigned long page)
{
    Word *current = (Word *) malloc(sizeof(Word));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->original = data;
    current->reduced = reduce_word(data, WO_SOURCE);
    current->filename = filename;
    current->line = line;
    current->column = column;
    current->position = position;
    current->page = page;
    current->field = full_text_field;
    current->next = NULL;
    current->prev = *list;
    if ((*list) != NULL)
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

char *
filename_word(Word *word)
{
    return word->filename;
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

unsigned long
page_word(Word *word)
{
    return word->page;
}

unsigned long
field_word(Word *word)
{
    if (word == NULL)
    {
        return end_field;
    }
    else
    {
        return word->field;
    }
}

bool
is_end_field(Word *word)
{
    if (field_word(word) == end_field)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
list_has_next_word(Word *word)
{
    if (is_end_field(word) == true)
    {
        return false;
    }
    else if (is_end_field(next_word(word)) == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
list_has_prev_word(Word *word)
{
    if (is_end_field(word) == true)
    {
        return false;
    }
    else if (is_end_field(prev_word(word)) == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
field_has_next_word(Word *word)
{
    if (is_end_field(word) == true)
    {
        return false;
    }
    else if (field_word(word) == field_word(next_word(word)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
field_has_prev_word(Word *word)
{
    if (is_end_field(word) == true)
    {
        return false;
    }
    else if (field_word(word) == field_word(prev_word(word)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
clause_ending_word(Word *word)
{
    if (is_end_field(word) == false)
    {
        if (sentence_ending_word(word) == true)
        {
            return true;
        }
        else
        {
            char *data = original_word(word);
            size_t len = strlen(data);
            bool curr_cond = false;
            if (len == 1)
            {
                curr_cond = is_clause_punctuation(data[len-1]);
            }
            else
            {
                curr_cond = (is_clause_punctuation(data[len-1]) ||
                    (
                        is_clause_punctuation(data[len-2])
                        &&
                        (
                            (data[len-1] == '"')
                            ||
                            (data[len-1] == '\'')
                        )
                    )
                );
            }
            return curr_cond;
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
    if (is_end_field(word) == false)
    {
        char *data = original_word(word);
        size_t len = strlen(data);
        bool curr_cond = false;
        if (len == 1)
        {
            curr_cond = is_ending_punctuation(data[len-1]);
        }
        else
        {
            curr_cond = (is_ending_punctuation(data[len-1]) ||
                (
                    is_ending_punctuation(data[len-2])
                    &&
                    (
                        (data[len-1] == '"')
                        ||
                        (data[len-1] == '\'')
                    )
                )
            );
        }
        if (field_has_next_word(word) == false)
        {
            return curr_cond;
        }
        else
        {
            char *next_data = original_word(next_word(word));
            size_t next_len = strlen(next_data);
            bool next_cond = false;
            if (next_len == 1)
            {
                next_cond = (isupper(next_data[0]) || isdigit(next_data[0]));
            }
            else
            {
                next_cond = (
                    (isupper(next_data[0]) || isdigit(next_data[0]))
                    ||
                    (
                        (isupper(next_data[1]) || isdigit(next_data[1]))
                        &&
                        (
                            (next_data[0] == '"')
                            ||
                            (next_data[0] == '\'')
                        )
                    )
                );
            }

            if ((curr_cond == true) && (next_cond == true))
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

bool
paragraph_ending_word(Word *word)
{
    if (is_end_field(word) == false)
    {
        bool sentence_cond = sentence_ending_word(word);
        if (field_has_next_word(word) == false)
        {
            return sentence_cond;
        }
        else
        {
            if ((sentence_cond == true) && (line_word(word) != line_word(next_word(word))))
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
next_boolean_element(Word *word, bool element_ending_word(Word *))
{
    if (is_end_field(word) == false)
    {
        Word *current = word;
        while (field_has_next_word(current) == true)
        {
            Word *next = next_word(current);
            if (element_ending_word(current) == true)
            {
                return next;
            }
            current = next;
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

Word *
prev_boolean_element(Word *word, bool element_ending_word(Word *))
{
    if (is_end_field(word) == false)
    {
        Word *current = word;
        if ((element_ending_word(current) == true) && (field_has_prev_word(current) == true))
        {
            current = prev_word(current);
        }
        while (field_has_prev_word(current) == true)
        {
            Word *prev = prev_word(current);
            if (element_ending_word(current) == true)
            {
                return next_word(current);
            }
            current = prev;
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

Word *
next_numbered_element(Word *word, unsigned long element_word(Word *))
{
    if (is_end_field(word) == false)
    {
        unsigned long element = element_word(word);
        Word *current = word;
        while (field_has_next_word(current) == true)
        {
            Word *next = next_word(current);
            if (element_word(current) != element)
            {
                return current;
            }
            current = next;
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

Word *
prev_numbered_element(Word *word, unsigned long element_word(Word *))
{
    if (is_end_field(word) == false)
    {
        unsigned long element = element_word(word);
        Word *current = word;
        while (field_has_prev_word(current) == true)
        {
            if (element_word(current) != element)
            {
                return next_word(current);
            }
            current = prev_word(current);
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

Word *
next_word(Word *word)
{
    if (is_end_field(word) == true)
    {
        return NULL;
    }
    else
    {
        return word->next;
    }
}

Word *
prev_word(Word *word)
{
    if (is_end_field(word) == true)
    {
        return NULL;
    }
    else
    {
        return word->prev;
    }
}

Word *
next_clause(Word *word)
{
    return next_boolean_element(word, clause_ending_word);
}

Word *
prev_clause(Word *word)
{
    return prev_boolean_element(word, clause_ending_word);
}

Word *
next_line(Word *word)
{
    return next_numbered_element(word, line_word);
}

Word *
prev_line(Word *word)
{
    return prev_numbered_element(word, line_word);
}

Word *
next_sentence(Word *word)
{
    return next_boolean_element(word, sentence_ending_word);
}

Word *
prev_sentence(Word *word)
{
    return prev_boolean_element(word, sentence_ending_word);
}

Word *
next_paragraph(Word *word)
{
    return next_boolean_element(word, paragraph_ending_word);
}

Word *
prev_paragraph(Word *word)
{
    return prev_boolean_element(word, paragraph_ending_word);
}

Word *
next_page(Word *word)
{
    return next_numbered_element(word, page_word);
}

Word *
prev_page(Word *word)
{
    return prev_numbered_element(word, page_word);
}

Word *
extreme_word(Word *word, bool has_another_word(Word *), Word *direction_word(Word *))
{
    if (is_end_field(word) == false)
    {
        Word *current = word;
        while (has_another_word(current) == true)
        {
            current = direction_word(current);
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

Word *
list_first_word(Word *word)
{
    return extreme_word(word, list_has_prev_word, prev_word);
}

Word *
list_last_word(Word *word)
{
    return extreme_word(word, list_has_next_word, next_word);
}

Word *
field_first_word(Word *word)
{
    return extreme_word(word, field_has_prev_word, prev_word);
}

Word *
field_last_word(Word *word)
{
    return extreme_word(word, field_has_next_word, next_word);
}

Word *
document_word(Word *word)
{
    return list_first_word(word);
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
        else if (element == LE_LINE && n > 0)
        {
            advance = next_line;
        }
        else if (element == LE_LINE && n < 0)
        {
            advance = prev_line;
        }
        else if (element == LE_SENTENCE && n > 0)
        {
            advance = next_sentence;
        }
        else if (element == LE_SENTENCE && n < 0)
        {
            advance = prev_sentence;
        }
        else if (element == LE_PARAGRAPH && n > 0)
        {
            advance = next_paragraph;
        }
        else if (element == LE_PARAGRAPH && n < 0)
        {
            advance = prev_paragraph;
        }
        else if (element == LE_PAGE && n > 0)
        {
            advance = next_page;
        }
        else if (element == LE_PAGE && n < 0)
        {
            advance = prev_page;
        }
        assert(advance != NULL);

        size_t m = abs(n);
        for (size_t i = 0; i < m; i++)
        {
            Word *next = advance(current);
            if (is_end_field(next) == false)
            {
                current = next;
            }
            else
            {
                if (n > 0)
                {
                    current = field_last_word(current);
                }
                else
                {
                    current = field_first_word(current);
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
    while (is_end_field(current) == false)
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
    while (is_end_field(current) == false)
    {
        Word *next = next_word(current);
        free(current->original);
        free(current->reduced);
        free(current);
        current = next;
    }
}
