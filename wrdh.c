/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct InputWord
{
    char *data;
    struct InputWord *next;
    struct InputWord *prev;
} InputWord;

void
append_word(InputWord **list, char *data)
{
    InputWord *current = (InputWord *) malloc(sizeof(InputWord));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->data = data;
    current->next = NULL;
    current->prev = *list;
    if ((*list) != NULL)
    {
        (*list)->next = current;
    }
    *list = current;
}

char *
data_word(InputWord *word)
{
    return word->data;
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
        free(list);
    }
}

bool
is_whitespace(char c)
{
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == EOF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void
read_input(InputWord **list)
{
    int c = getchar();
    while (c != EOF)
    {
        size_t len = 0;
        char *data = NULL;
        while (is_whitespace(c) == false)
        {
            len++;
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
            append_word(list, data);
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
