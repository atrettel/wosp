/* Copyright (C) 2025 Andrew Trettel */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>

#include "search.h"

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

typedef struct SyntaxTree
{
    TokenType type;
    int n;
    char *string;
    struct SyntaxTree *left;
    struct SyntaxTree *right;
} SyntaxTree;

TokenType type_syntax_tree(SyntaxTree *);
int number_syntax_tree(SyntaxTree *);
char *string_syntax_tree(SyntaxTree *);
SyntaxTree *left_syntax_tree(SyntaxTree *);
SyntaxTree *right_syntax_tree(SyntaxTree *);

SyntaxTree *insert_parent(TokenType, int, char *, SyntaxTree *, SyntaxTree *);
void print_syntax_tree(SyntaxTree *);
void free_syntax_tree(SyntaxTree *);

/*
 * <Query>      -> <Expression> {AND <Expression> | OR <Expression> | NOT <Expression> | XOR <Expression>}
 * <Expression> -> <Phrase> {NEARn <Phrase> | WITHn <Phrase>}
 * <Phrase>     -> <Atom> {ADJn <Atom>} | "wildcard {wildcard}"
 * <Atom>       -> wildcard | (<Query>)
*/
SyntaxTree *parse_query(Token *);
SyntaxTree *parse_expression(Token *);
SyntaxTree *parse_phrase(Token *);
SyntaxTree *parse_atom(Token *);

Match *eval_syntax_tree(SyntaxTree *);

#endif /* INTERPRETER_H */
