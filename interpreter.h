/* Copyright (C) 2025 Andrew Trettel */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>

typedef enum TokenType
{                /* Example */
    TK_WILDCARD, /* w?rd$1 */
    TK_QUOTE,    /* " or ' */
    TK_L_PAREN,  /* ( */
    TK_R_PAREN,  /* ) */
    TK_OR_OP,    /* OR */
    TK_AND_OP,   /* AND */
    TK_NOT_OP,   /* NOT */
    TK_XOR_OP,   /* XOR */
    TK_ADJ_OP,   /* ADJ5 */
    TK_NEAR_OP,  /* NEAR5 */
    TK_WITH_OP,  /* WITH5 */
    TK_ERROR     /* This should not happen. */
} TokenType;

typedef struct Token
{
    TokenType type;
    int n;
    char *string;
    struct Token *prev;
    struct Token *next;
    unsigned int cumulative_quotes;
} Token;

void insert_token(Token **, TokenType, int, char *);
TokenType type_token(Token *);
int number_token(Token *);
char *string_token(Token *);
Token *prev_token(Token *);
Token *next_token(Token *);
Token *first_token(Token *);
Token *last_token(Token *);
void free_tokens(Token *);

Token *lex_query(char *);
unsigned int count_errors_tokens(Token *, bool);

#endif /* INTERPRETER_H */
