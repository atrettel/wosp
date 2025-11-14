// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "misc.h"
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
read_source_words(Word **list, FILE *stream, char *filename)
{
    unsigned long line = 1, column = 1, position = 1;
    int p = '\0';
    int c = fgetc(stream);
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
                data = (char *) allocmem(len, sizeof(char));
            }
            else
            {
                char *tmp = (char *) reallocmem(data, len);
                data = tmp;
            }
            data[len-1] = (char) c;
            p = c;
            c = fgetc(stream);
        }
        if (len > 0)
        {
            len++;
            char *tmp = (char *) reallocmem(data, len);
            data = tmp;
            data[len-1] = '\0';
            append_word(list, data, filename, line, column, position, 1);
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
        c = fgetc(stream);
    }
    *list = list_first_word(*list);
}

size_t
read_data(int argc, char *argv[], TrieNode **trie, char ***filenames, Word ***words)
{
    if (argc == 1)
    {
        fprintf(stderr, "%s: No query given\n", program_name);
        exit(EXIT_FAILURE);
    }

    size_t n_files = (argc == 2) ? 1 : argc - 2;
    *filenames = (char **) allocmem(n_files, sizeof(char *));
    *words = (Word **) allocmem(n_files, sizeof(Word *));
    for (size_t i = 0; i < n_files; i++)
    {
        (*filenames)[i] = NULL;
        (*words)[i] = NULL;
    }
    if (argc == 2)
    {
        (*filenames)[0] = (char *) allocmem(6, sizeof(char));
        snprintf((*filenames)[0], 6, "stdin");
    }
    else
    {
        for (size_t i = 0; i < n_files; i++)
        {
            (*filenames)[i] = (char *) allocmem((strlen(argv[i+2])+1), sizeof(char));
            snprintf((*filenames)[i], strlen(argv[i+2])+1, "%s", argv[i+2]);
        }
    }
    init_trie(trie);
    for (size_t i = 0; i < n_files; i++)
    {
        (*words)[i] = NULL;
        if (argc == 2)
        {
            read_source_words(&((*words)[i]), stdin, (*filenames)[i]);
        }
        else
        {
            FILE *f = fopen((*filenames)[i], "r");
            if (f == NULL)
            {
                fprintf(stderr, "%s: File '%s' does not exist\n", program_name, (*filenames)[i]);
                exit(EXIT_FAILURE);
            }
            read_source_words(&((*words)[i]), f, (*filenames)[i]);
            fclose(f);
        }
        add_words_to_trie(*trie, (*words)[i]);
    }

    return n_files;
}

void
free_data(size_t n_files, TrieNode *trie, char **filenames, Word **words)
{
    free_trie(trie);
    for (size_t i = 0; i < n_files; i++)
    {
        free(filenames[i]);
        free_words(words[i]);
    }
    free(filenames);
    free(words);
}

