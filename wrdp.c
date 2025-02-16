/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
is_line_break(char c)
{
    if (c == '\n' || c == '\r')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
is_whitespace(char c)
{
    if (is_line_break(c) || c == ' ' || c == '\t' || c == EOF)
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
is_punctuation(char c)
{
    if (is_ending_punctuation(c) ||
        c == ',' || c == ':' || c == ';' ||
        c == '-' || c == '(' || c == ')')
    {
        return true;
    }
    else
    {
        return false;
    }
}

typedef struct InputWord
{
    char *original;
    char *reduced; /* The lowercase word without any punctuation */
    unsigned int line;
    unsigned int column;
    bool ends_sentence;
    struct InputWord *next;
    struct InputWord *prev;
} InputWord;

char *
reduce_word(char *original)
{
    size_t len = 0, j = 0;
    for (size_t i = 0; i < strlen(original); i++)
    {
        if (is_punctuation(original[i]) == false )
        {
            len++;
        }
    }
    len++;
    char *reduced = (char *) malloc(len * sizeof(char));
    for (size_t i = 0; i < strlen(original); i++)
    {
        if (is_punctuation(original[i]) == false )
        {
            reduced[j] = tolower(original[i]);
            j++;
        }
    }
    return reduced;
}

void
append_word(InputWord **list, char *data, unsigned int line, unsigned int column)
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
    current->ends_sentence = is_ending_punctuation(data[strlen(data)-1]);
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

unsigned int
line_word(InputWord *word)
{
    return word->line;
}

unsigned int
column_word(InputWord *word)
{
    return word->column;
}

bool
sentence_ending_word(InputWord *word)
{
    return word->ends_sentence;
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

void
read_input(InputWord **list)
{
    unsigned int line = 1, column = 1;
    int c = getchar();
    while (c != EOF)
    {
        size_t len = 0;
        char *data = NULL;
        while (is_whitespace(c) == false)
        {
            len++;
            column++;
            if (data == NULL)
            {
                data = (char *) malloc(len * sizeof(char));
            }
            else
            {
                data = (char *) realloc(data, len);

            }
            if (data == NULL)
            {
                exit(EXIT_FAILURE);
            }
            data[len-1] = (char) c;
            c = getchar();
        }
        if (len > 0)
        {
            len++;
            data = (char *) realloc(data, len);
            if (data == NULL)
            {
                exit(EXIT_FAILURE);
            }
            data[len+1] = '\0';
            append_word(list, data, line, column);
        }
        if (is_line_break(c) == true)
        {
            line++;
            column = 1;
        }
        else if (is_whitespace(c) == true)
        {
            column++;
        }
        c = getchar();
    }
}

int
main(void)
{
    InputWord *list = NULL;
    read_input(&list);
    free_word(list);
    return EXIT_SUCCESS;
}
