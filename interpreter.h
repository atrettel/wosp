/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2025 Andrew Trettel */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include <stdio.h>

#include "output.h"
#include "search.h"

static const bool debug_syntax_tree = false;

typedef enum TokenType
{                /* Example */
    TK_WILDCARD, /* w?rd$1 */
    TK_QUOTE,    /* " or ' */
    TK_L_PAREN,
    TK_R_PAREN,
    TK_OR_OP,
    TK_AND_OP,
    TK_NOT_OP,
    TK_XOR_OP,
    TK_ADJ_OP,
    TK_NEAR_OP,
    TK_AMONG_OP,
    TK_ALONG_OP,
    TK_WITH_OP,
    TK_SAME_OP,
    TK_NOT_ADJ_OP,
    TK_NOT_NEAR_OP,
    TK_NOT_AMONG_OP,
    TK_NOT_ALONG_OP,
    TK_NOT_WITH_OP,
    TK_NOT_SAME_OP,
    TK_ICASE_OP,
    TK_SCASE_OP,
    TK_LCASE_OP,
    TK_UCASE_OP,
    TK_TCASE_OP,
    TK_ERROR
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

typedef struct TokenIterator
{
    Token *next;
} TokenIterator;

void insert_token(Token **, TokenType, int, char *, TokenType);
TokenType type_token(Token *);
int number_token(Token *);
char *string_token(Token *);
Token *prev_token(Token *);
Token *next_token(Token *);
Token *first_token(Token *);
bool is_token(Token *);
bool has_prev_token(Token *);
void free_tokens(Token *);

TokenType find_operator_type(char *);
const char *find_operator_prefix(TokenType);
void identify_token_type(char *, TokenType *, int *);

TokenIterator init_token_iterator(Token *);
Token *iterator_next_token(TokenIterator *);
bool iterator_has_next_token(TokenIterator);

Token *lex_query(char *, TokenType);
bool operator_token_type(TokenType);
bool boolean_operator_token_type(TokenType);
bool proximity_operator_token_type(TokenType);
bool search_operator_token_type(TokenType);
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
void print_syntax_tree(FILE *, SyntaxTree *, bool);
void free_syntax_tree(SyntaxTree *);

bool type_in_list(TokenType, TokenType *, size_t);
SyntaxTree *parse_types(Token **, TokenType *, size_t, SyntaxTree *parse_next(Token **));

SyntaxTree *parse_query(Token **);
SyntaxTree *parse_disjunction_op(Token **);
SyntaxTree *parse_conjunction_op(Token **);
SyntaxTree *parse_negation_op(Token **);
SyntaxTree *parse_paragraph_prox_op(Token **);
SyntaxTree *parse_sentence_prox_op(Token **);
SyntaxTree *parse_clause_prox_op(Token **);
SyntaxTree *parse_word_prox_op(Token **);
SyntaxTree *parse_adj_op(Token **);
SyntaxTree *parse_search_op(Token **);
SyntaxTree *parse_atom(Token **);

Match *eval_syntax_tree(SyntaxTree *, TrieNode *, CaseMode, unsigned int, ProximityMode, bool *);
void interpret_query(char *, TrieNode *, CaseMode, unsigned int, ProximityMode, TokenType, OutputOptions);

#endif /* INTERPRETER_H */
