/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "interpreter.h"

#include <stdio.h>

void
insert_token(Token **list, TokenType type, int n, char *string)
{
    unsigned int cumulative_quotes = 0;
    if (*list != NULL)
    {
        cumulative_quotes = (*list)->cumulative_quotes;
    }
    TokenType prev_type = type_token(*list);
    if ((     (type == WILDCARD) ||      (type == QUOTE) ||      (type == L_PAREN)) &&
        ((prev_type == WILDCARD) || (prev_type == QUOTE) || (prev_type == R_PAREN))
        && (cumulative_quotes % 2 == 0))
    {
        char *tmp = malloc(3 * sizeof(char));
        tmp[0] = 'O';
        tmp[1] = 'R';
        tmp[2] = '\0';
        insert_token(list, OP_OR, 0, tmp); /* Default operation */
    }
    Token *current = (Token *) malloc(sizeof(Token));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->type = type;
    current->n = n;
    current->string = string;
    current->next = NULL;
    current->prev = *list;
    if (current->prev == NULL)
    {
        current->cumulative_quotes = 0;
    }
    else if (type == QUOTE)
    {
        current->cumulative_quotes = current->prev->cumulative_quotes + 1;
    }
    else
    {
        current->cumulative_quotes = current->prev->cumulative_quotes;
    }
    if ((*list) != NULL)
    {
        (*list)->next = current;
    }
    *list = current;
}

TokenType
type_token(Token *token)
{
    if (token == NULL)
    {
        return ERROR_TOKEN;
    }
    else
    {
        return token->type;
    }
}

int
number_token(Token *token)
{
    if (token == NULL)
    {
        return 0;
    }
    else
    {
        return token->n;
    }
}

char *
string_token(Token *token)
{
    if (token == NULL)
    {
        return NULL;
    }
    else
    {
        return token->string;
    }
}

Token *
prev_token(Token *token)
{
    if (token == NULL)
    {
        return NULL;
    }
    else
    {
        return token->prev;
    }
}

Token *
next_token(Token *token)
{
    if (token == NULL)
    {
        return NULL;
    }
    else
    {
        return token->next;
    }
}

Token *
first_token(Token *token)
{
    if (token == NULL)
    {
        return NULL;
    }
    else
    {
        Token *current = token;
        while (prev_token(current) != NULL)
        {
            current = prev_token(current);
        }
        return current;
    }
}

Token *
last_token(Token *token)
{
    if (token == NULL)
    {
        return NULL;
    }
    else
    {
        Token *current = token;
        while (next_token(current) != NULL)
        {
            current = next_token(current);
        }
        return current;
    }
}

void
free_tokens(Token *list)
{
    Token *current = list;
    while (current != NULL)
    {
        Token *next = next_token(current);
        free(current->string);
        free(current);
        current = next;
    }
}

void
get_token_type(char *data, TokenType *type, int *n)
{
    size_t len = strlen(data);
    size_t j = len; /* Where the numbers start, length of prefix */
    char *lcase = (char *) malloc((len + 1) * sizeof(char));
    for (size_t i = 0; i < len; i++)
    {
        lcase[i] = tolower(data[i]);
        if (isdigit(lcase[i]) && (i < j))
        {
            j = i;
        }
    }
    size_t k = len - j; /* Length of suffix */
    lcase[len] = '\0';
    char *prefix = (char *) malloc((j + 1) * sizeof(char));
    for (size_t i = 0; i < j; i++)
    {
        prefix[i] = lcase[i];
    }
    prefix[j] = '\0';
    char *suffix = (char *) malloc((k + 1) * sizeof(char));
    bool has_nondigits = false;
    for (size_t i = 0; i < k; i++)
    {
        suffix[i] = lcase[i+j];
        if (!isdigit(suffix[i]))
        {
            has_nondigits = true;
        }
    }
    suffix[k] = '\0';
    char *endptr;
    long m = strtol(suffix, &endptr, 10);

    if (j < 2)
    {
        *type = WILDCARD;
        *n = 0;
    }
    else if (j < 3)
    {
        if ((strncmp(prefix, "or", 2) == 0) && (k == 0))
        {
            *type = OP_OR;
            *n = 0;
        }
    }
    else if (j < 4)
    {
        if ((strncmp(prefix, "and", 3) == 0) && (k == 0))
        {
            *type = OP_AND;
            *n = 0;
        }
        else if ((strncmp(prefix, "not", 3) ==0) && (k == 0))
        {
            *type = OP_NOT;
            *n = 0;
        }
        else if ((strncmp(prefix, "xor", 3) == 0) && (k == 0))
        {
            *type = OP_XOR;
            *n = 0;
        }
        else if ((strncmp(prefix, "adj", 3) == 0) && (has_nondigits == false))
        {
            *type = OP_ADJ;
            if (k == 0)
            {
                *n = 1;
            }
            else
            {
                *n = (int) m;
            }
        }
    }
    else
    {
        if ((strncmp(prefix, "near", 4) == 0) && (has_nondigits == false))
        {
            *type = OP_NEAR;
            if (k == 0)
            {
                *n = 1;
            }
            else
            {
                *n = (int) m;
            }
        }
        else if ((strncmp(prefix, "with", 4) == 0) && (has_nondigits == false))
        {
            *type = OP_WITH;
            if (k == 0)
            {
                *n = 1;
            }
            else
            {
                *n = (int) m;
            }
        }
        else if ((strncmp(prefix, "or",   2) != 0) &&
                 (strncmp(prefix, "and",  3) != 0) &&
                 (strncmp(prefix, "not",  3) != 0) &&
                 (strncmp(prefix, "xor",  3) != 0) &&
                 (strncmp(prefix, "adj",  3) != 0) &&
                 (strncmp(prefix, "near", 4) != 0) &&
                 (strncmp(prefix, "with", 4) != 0))
        {
            *type = WILDCARD;
            *n = 0;
        }
    }
    free(lcase);
    free(prefix);
    free(suffix);
}

Token *
lex_query(char *query)
{
    Token *tokens = NULL;
    size_t i = 0;
    size_t n = strlen(query);
    while (i < n)
    {
        if ((query[i] == '(') || (query[i] == ')') || (query[i] == '"') || (query[i] == '\''))
        {
            char *data = (char *) malloc(2 * sizeof(char));
            data[0] = query[i];
            data[1] = '\0';
            TokenType type = ERROR_TOKEN;
            if (query[i] == '(')
            {
                type = L_PAREN;
            }
            else if (query[i] == ')')
            {
                type = R_PAREN;
            }
            else if ((query[i] == '"') || (query[i] == '\''))
            {
                type = QUOTE;
            }
            insert_token(&tokens, type, 0, data);
            i++;
        }
        size_t len = 1;
        char *data = (char *) malloc(len * sizeof(char));
        if (data == NULL)
        {
            exit(EXIT_FAILURE);
        }
        data[len - 1] = '\0';
        while ((isspace(query[i]) == false) && (query[i] != ')') && (query[i] != '"') && (query[i] != '\'') && (i < n))
        {
            len++;
            data = (char *) realloc(data, len);
            data[len - 2] = query[i];
            data[len - 1] = '\0';
            i++;
        }
        if (len > 1)
        {
            TokenType type = ERROR_TOKEN;
            int n = -1;
            get_token_type(data, &type, &n);
            insert_token(&tokens, type, n, data);
        }
        else
        {
            free(data);
        }
        if ((query[i] != ')') && (query[i] != '"') && (query[i] != '\''))
        {
            i++;
        }
    }
    return first_token(tokens);
}
