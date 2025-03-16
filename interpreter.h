/* Copyright (C) 2025 Andrew Trettel */
#ifndef INTERPRETER_H
#define INTERPRETER_H

typedef enum TokenType
{               /* Example */
    WILDCARD,   /* w?rd$1 */
    QUOTE,      /* "first second" */
    L_PAREN,    /* ( */
    R_PAREN,    /* ) */
    OP_OR,      /* OR */
    OP_AND,     /* AND */
    OP_NOT,     /* NOT */
    OP_XOR,     /* XOR */
    OP_ADJ,     /* ADJ5 */
    OP_NEAR,    /* NEAR5 */
    OP_WITH,    /* WITH5 */
    ERROR_TOKEN /* This should not happen. */
} TokenType;

typedef struct Token
{
    TokenType type;
    char *string;
    struct Token *prev;
    struct Token *next;
} Token;

void insert_token(Token **, TokenType, char *);
TokenType type_token(Token *);
char *string_token(Token *);
Token *prev_token(Token *);
Token *next_token(Token *);
Token *first_token(Token *);
Token *last_token(Token *);
void free_tokens(Token *);

Token *lex_query(char *);

#endif /* INTERPRETER_H */
