/* Copyright (C) 2025 Andrew Trettel */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "search.h"
#include "words.h"

void
build_trie(TrieNode **trie, InputWord *list)
{
    init_trie(trie);
    InputWord *current = list;
    while (current != NULL)
    {
        insert_trie(*trie, current, 0);
        current = next_word(current);
    }
}

void
read_input_words(InputWord **list)
{
    unsigned long line = 1, column = 1, position = 1;
    int c = getchar();
    while (c != EOF)
    {
        size_t len = 0;
        char *data = NULL;
        while ((isspace(c) == false) && (c != EOF))
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
            data[len-1] = '\0';
            append_word(list, data, line, column, position);
            position++;
        }
        if (c == '\n' || c == '\r')
        {
            line++;
            column = 1;
        }
        else if (isblank(c) == true)
        {
            column++;
        }
        c = getchar();
    }
    *list = first_word(*list);
}

int
main(void)
{
    InputWord *list = NULL;
    TrieNode *trie = NULL;

    read_input_words(&list);
    build_trie(&trie, list);

    print_words(list);

    free_trie(trie);
    free_words(list);
    return EXIT_SUCCESS;
}
