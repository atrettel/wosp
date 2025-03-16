/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdlib.h>

#include "config.h"
#include "interpreter.h"

void
insert_token(Token **list, TokenType type, char *string)
{
    TokenType prev_type = type_token(*list);
    if (
        ((type == WILDCARD) ||
         (type == QUOTE)    ||
         (type == L_PAREN)) &&
        ((prev_type == WILDCARD) || 
         (prev_type == QUOTE)    ||
         (prev_type == R_PAREN))
        )
    {
        /* Default operation */
        insert_token(list, OP_OR, "OR");
    }
    Token *current = (Token *) malloc(sizeof(Token));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->type = type;
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
        /*free(current->string);*/
        free(current);
        current = next;
    }
}
