/* Copyright (C) 2025 Andrew Trettel */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "operations.h"
#include "search.h"
#include "words.h"

void
add_words_to_trie(TrieNode *trie, Word *list)
{
    Word *current = list;
    while (current != NULL)
    {
        insert_trie(trie, current, 0);
        current = next_word(current);
    }
}

void
read_source_words(Word **list)
{
    unsigned long line = 1, column = 1, position = 1;
    int p = '\0';
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
            p = c;
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
        if ((p != '\r' && c == '\n') || c == '\r')
        {
            line++;
            column = 1;
        }
        else if (isblank(c) == true)
        {
            column++;
        }
        p = c;
        c = getchar();
    }
    *list = first_word(*list);
}

int
main(void)
{
    Word *list = NULL;
    TrieNode *trie = NULL;

    init_trie(&trie);
    read_source_words(&list);
    add_words_to_trie(trie, list);

    Match *first_match = wildcard_search(trie, "law$1");
    Match *second_match = wildcard_search(trie, "assent");
    Match *match = op_with(first_match, second_match, 1);
    free_matches(first_match);
    free_matches(second_match);

    print_matches(match);
    free_matches(match);

    free_trie(trie);
    free_words(list);
    return EXIT_SUCCESS;
}
