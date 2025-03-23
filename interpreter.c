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
        char *tmp = malloc(sizeof(default_operation_string));
        if (tmp == NULL)
        {
            exit(EXIT_FAILURE);
        }
        snprintf(tmp, sizeof(default_operation_string), "%s", default_operation_string);
        insert_token(list, default_operation_type, 0, tmp);
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

static const char  *operator_prefixes[] = { "or",  "and",  "not",  "xor",  "adj",  "near",  "with"};
static const TokenType operator_types[] = {OP_OR, OP_AND, OP_NOT, OP_XOR, OP_ADJ, OP_NEAR, OP_WITH};

TokenType
find_operator_type(char *prefix)
{
    TokenType type = ERROR_TOKEN;
    size_t n = sizeof(operator_types) / sizeof(operator_types[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (strncmp(prefix, operator_prefixes[i], strlen(prefix)) == 0)
        {
            type = operator_types[i];
            break;
        }
    }
    return type;
}

void
identity_token_type(char *data, TokenType *type, int *n)
{
    size_t len = strlen(data);
    size_t j = len; /* Where the numbers start, length of prefix */
    char *lcase = (char *) malloc((len + 1) * sizeof(char));
    if (lcase == NULL)
    {
        exit(EXIT_FAILURE);
    }
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
    if (prefix == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < j; i++)
    {
        prefix[i] = lcase[i];
    }
    prefix[j] = '\0';
    char *suffix = (char *) malloc((k + 1) * sizeof(char));
    if (suffix == NULL)
    {
        exit(EXIT_FAILURE);
    }
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

    TokenType prefix_type = find_operator_type(prefix);
    if (((prefix_type == OP_OR)  ||
         (prefix_type == OP_AND) ||
         (prefix_type == OP_NOT) ||
         (prefix_type == OP_XOR)) && (k == 0))
    {
        *type = prefix_type;
        *n = 0;
    }
    else if (((prefix_type == OP_ADJ)  ||
              (prefix_type == OP_NEAR) ||
              (prefix_type == OP_WITH)) && (has_nondigits == false))
    {
        *type = prefix_type;
        if (k == 0)
        {
            *n = 1;
        }
        else
        {
            *n = (int) m;
        }
    }
    else
    {
        *type = WILDCARD;
        *n = 0;
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
            char *tmp = (char *) malloc(2 * sizeof(char));
            if (tmp == NULL)
            {
                exit(EXIT_FAILURE);
            }
            tmp[0] = query[i];
            tmp[1] = '\0';
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
            insert_token(&tokens, type, 0, tmp);
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
            identity_token_type(data, &type, &n);
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

unsigned int
count_errors_tokens(Token *list, bool print_errors)
{
    unsigned int n = 0;
    unsigned int n_quotes = 0;
    unsigned int n_l_parens = 0;
    unsigned int n_r_parens = 0;
    Token *current = list;
    while (current != NULL)
    {
        TokenType type = type_token(current);
        if (type == ERROR_TOKEN)
        {
            n++;
            if (print_errors)
            {
                fprintf(stderr, "(%u) error in token '%s'\n", n, string_token(current));
            }
        }
        else if (type == QUOTE)
        {
            n_quotes++;
        }
        else if (type == L_PAREN)
        {
            n_l_parens++;
        }
        else if (type == R_PAREN)
        {
            n_r_parens++;
        }
        current = next_token(current);
    }

    if (n_quotes % 2 != 0)
    {
        n++;
        if (print_errors)
        {
            printf("(%u) unbalanced quotation marks present\n", n);
        }
    }
    if (n_l_parens != n_r_parens)
    {
        n++;
        if (print_errors)
        {
            printf("(%u) unbalanced parentheses present\n", n);
        }
    }

    return n;
}
