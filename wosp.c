/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
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
read_source_words(Word **list, FILE *stream)
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
            c = fgetc(stream);
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
        c = fgetc(stream);
    }
    *list = first_word(*list);
}

int
main(int argc, char *argv[])
{
    TrieNode *trie = NULL;
    init_trie(&trie);

    if (argc == 0)
    {
        fprintf(stderr, "No query given.\n");
        return EXIT_FAILURE;
    }

    size_t n_files = (argc == 2) ? 1 : argc - 2;
    printf("argc = %d, n_files = %lu\n", argc, n_files);
    char **filenames = (char **) malloc(n_files * sizeof(char *));
    Word **words = malloc(n_files * sizeof(Word *));
    if (filenames == NULL || words == NULL)
    {
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < n_files; i++)
    {
        filenames[i] = NULL;
        words[i] = NULL;
    }
    if (argc == 2)
    {
        filenames[0] = (char *) malloc(6 * sizeof(char));
        if (filenames[0] == NULL)
        {
            return EXIT_FAILURE;
        }
        snprintf(filenames[0], 6, "stdin");
    }
    else
    {
        for (size_t i = 0; i < n_files; i++)
        {
            filenames[i] = (char *) malloc((strlen(argv[i+2])+1) * sizeof(char *));
            if (filenames[i] == NULL)
            {
                return EXIT_FAILURE;
            }
            snprintf(filenames[i], strlen(argv[i+2])+1, "%s", argv[i+2]);
        }
    }
    for (size_t i = 0; i < n_files; i++)
    {
        printf("Filename = %s\n", filenames[i]);
        words[i] = NULL;
        if (argc == 2)
        {
            read_source_words(&words[i], stdin);
        }
        else
        {
            FILE *f = fopen(filenames[i], "r");
            read_source_words(&words[i], f);
            fclose(f);
        }
        add_words_to_trie(trie, words[i]);
    }

    Token *tokens = lex_query(argv[1]);
    unsigned int n_errors = count_errors_tokens(tokens, true);

    printf("Lex query into tokens\n");
    Token *current = tokens;
    while (is_token(current) == true)
    {
        printf("%s - %d\n", string_token(current), type_token(current));
        current = next_token(current);
    }

    if (n_errors == 0)
    {
        printf("Parse tokens into syntax tree\n");
        SyntaxTree *tree = parse_query(&tokens);
        print_syntax_tree(tree);
        printf("\n");
        printf("Evaluate syntax tree\n");
        bool error_flag = false;
        Match *matches = eval_syntax_tree(tree, trie, &error_flag);
        free_syntax_tree(tree);
        if (error_flag == false)
        {
            print_matches(matches);
            free_matches(matches);
        }
        else
        {
            fprintf(stderr, "At least one syntax error present\n");
        }
    }
    else
    {
        fprintf(stderr, "One of more errors in query\n");
    }

    free_tokens(tokens);

    free_trie(trie);
    for (size_t i = 0; i < n_files; i++)
    {
        free(filenames[i]);
        free_words(words[i]);
    }
    free(filenames);
    free(words);
    return EXIT_SUCCESS;
}
