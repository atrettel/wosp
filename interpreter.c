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
    TokenType prev_type = type_token(*list);
    if ((     (type == WILDCARD) ||      (type == QUOTE) ||      (type == L_PAREN)) &&
        ((prev_type == WILDCARD) || (prev_type == QUOTE) || (prev_type == R_PAREN)))
    {
        insert_token(list, OP_OR, 0, "OR"); /* Default operation */
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

Token *
lex_query(char *query)
{
    Token *tokens = NULL;
    size_t i = 0;
    size_t n = strlen(query);
    while (i < n)
    {
        if ((query[i] == '(') || (query[i] == ')'))
        {
            char *data = (char *) malloc(2 * sizeof(char));
            data[0] = query[i];
            data[1] = '\0';
            insert_token(&tokens, (query[i] == '(' ? L_PAREN : R_PAREN), 0, data);
            printf("data = '%s'\n", data);
            i++;
        }
        size_t len = 1;
        char *data = (char *) malloc(len * sizeof(char));
        if (data == NULL)
        {
            exit(EXIT_FAILURE);
        }
        data[len - 1] = '\0';
        while ((isspace(query[i]) == false) && (query[i] != ')') && (i < n))
        {
            len++;
            data = (char *) realloc(data, len);
            data[len - 2] = query[i];
            data[len - 1] = '\0';
            i++;
        }
        if (len > 1)
        {
            data[len - 1] = '\0';
            insert_token(&tokens, ERROR_TOKEN, 0, data);
        }
        if (query[i] != ')')
        {
            i++;
        }
    }
    return first_token(tokens);
}
