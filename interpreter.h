/* Copyright (C) 2025 Andrew Trettel */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include <stdio.h>

#include "search.h"

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

void insert_token(Token **, TokenType, int, char *);
TokenType type_token(Token *);
int number_token(Token *);
char *string_token(Token *);
Token *prev_token(Token *);
Token *next_token(Token *);
Token *first_token(Token *);
Token *last_token(Token *);
bool is_token(Token *);
bool has_next_token(Token *);
bool has_prev_token(Token *);
void free_tokens(Token *);

TokenType find_operator_type(char *);
const char *find_operator_prefix(TokenType);
void identify_token_type(char *, TokenType *, int *);

Token *lex_query(char *);
bool operator_token_type(TokenType);
bool boolean_operator_token_type(TokenType);
bool proximity_operator_token_type(TokenType);
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

/*
 * <Query>      -> <Expression A> {AND <Expression A> | OR <Expression A> | NOT <Expression A> | XOR <Expression A>}
 * <Expression A> -> <Expression B> {SAMEn <Expression B>}
 * <Expression B> -> <Expression C> {WITHn <Expression C> | ALONGn <Expression C>}
 * <Expression C> -> <Expression D> {AMONGn <Expression D>}
 * <Expression D> -> <Expression E> {NEARn <Expression E>}
 * <Expression E> -> <Atom> {ADJn <Atom>}
 * <Atom>       -> wildcard | (<Query>) | "wildcard {wildcard}"
*/
SyntaxTree *parse_query(Token **);
SyntaxTree *parse_expression_a(Token **);
SyntaxTree *parse_expression_b(Token **);
SyntaxTree *parse_expression_c(Token **);
SyntaxTree *parse_expression_d(Token **);
SyntaxTree *parse_expression_e(Token **);
SyntaxTree *parse_atom(Token **);

Match *eval_syntax_tree(SyntaxTree *, TrieNode *, bool, bool *);
void interpret_query(char *, TrieNode *, bool);

#endif /* INTERPRETER_H */
