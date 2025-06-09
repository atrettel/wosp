/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

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
    Word *list = NULL;
    TrieNode *trie = NULL;

    init_trie(&trie);
    read_source_words(&list, stdin);
    add_words_to_trie(trie, list);

    assert(argc > 1);
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
    free_words(list);
    return EXIT_SUCCESS;
}
