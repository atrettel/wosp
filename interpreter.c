/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "interpreter.h"
#include "operations.h"

#include <stdio.h>

void
insert_token(Token **list, TokenType type, int n, char *string)
{
    unsigned int cumulative_quotes = (type == TK_QUOTE) ? 1 : 0;
    if (is_token(*list) == true)
    {
        cumulative_quotes = (*list)->cumulative_quotes;
    }
    TokenType prev_type = type_token(*list);
    if ((     (type == TK_WILDCARD) ||      (type == TK_QUOTE) ||      (type == TK_L_PAREN)) &&
        ((prev_type == TK_WILDCARD) || (prev_type == TK_QUOTE) || (prev_type == TK_R_PAREN))
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
    if (is_token(current->prev) == false)
    {
        current->cumulative_quotes = (type == TK_QUOTE) ? 1 : 0;
    }
    else if (type == TK_QUOTE)
    {
        current->cumulative_quotes = current->prev->cumulative_quotes + 1;
    }
    else
    {
        current->cumulative_quotes = current->prev->cumulative_quotes;
    }
    if (is_token(*list) == true)
    {
        (*list)->next = current;
    }
    *list = current;
}

TokenType
type_token(Token *token)
{
    if (is_token(token) == true)
    {
        return token->type;
    }
    else
    {
        return TK_ERROR;
    }
}

int
number_token(Token *token)
{
    if (is_token(token) == true)
    {
        return token->n;
    }
    else
    {
        return 0;
    }
}

char *
string_token(Token *token)
{
    if (is_token(token) == true)
    {
        return token->string;
    }
    else
    {
        return NULL;
    }
}

Token *
prev_token(Token *token)
{
    if (is_token(token) == true)
    {
        return token->prev;
    }
    else
    {
        return NULL;
    }
}

Token *
next_token(Token *token)
{
    if (is_token(token) == true)
    {
        return token->next;
    }
    else
    {
        return NULL;
    }
}

Token *
first_token(Token *token)
{
    if (is_token(token) == true)
    {
        Token *current = token;
        while (has_prev_token(current) == true)
        {
            current = prev_token(current);
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

Token *
last_token(Token *token)
{
    if (is_token(token) == true)
    {
        Token *current = token;
        while (has_next_token(current) == true)
        {
            current = next_token(current);
        }
        return current;
    }
    else
    {
        return NULL;
    }
}

bool
is_token(Token *token)
{
    if (token == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
has_next_token(Token *token)
{
    if ((is_token(           token)  == false) ||
        (is_token(next_token(token)) == false))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
has_prev_token(Token *token)
{
    if ((is_token(           token)  == false) ||
        (is_token(prev_token(token)) == false))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void
free_tokens(Token *list)
{
    Token *current = list;
    while (is_token(current) == true)
    {
        Token *next = next_token(current);
        free(current->string);
        free(current);
        current = next;
    }
}

static const char  *operator_prefixes[] = {    "or",     "and",     "not",     "xor",     "adj",     "near",     "among",     "along",     "with",     "same"};
static const TokenType operator_types[] = {TK_OR_OP, TK_AND_OP, TK_NOT_OP, TK_XOR_OP, TK_ADJ_OP, TK_NEAR_OP, TK_AMONG_OP, TK_ALONG_OP, TK_WITH_OP, TK_SAME_OP};

TokenType
find_operator_type(char *prefix)
{
    TokenType type = TK_ERROR;
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

const char *
find_operator_prefix(TokenType type)
{
    size_t n = sizeof(operator_types) / sizeof(operator_types[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (operator_types[i] == type)
        {
            return operator_prefixes[i];
        }
    }
    return NULL;
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
    char *suffix = (char *) malloc((k + 1) * sizeof(char));
    if ((prefix == NULL) || (suffix == NULL))
    {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < j; i++)
    {
        prefix[i] = lcase[i];
    }
    prefix[j] = '\0';
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
    if (((prefix_type == TK_OR_OP)  ||
         (prefix_type == TK_AND_OP) ||
         (prefix_type == TK_NOT_OP) ||
         (prefix_type == TK_XOR_OP)) && (k == 0))
    {
        *type = prefix_type;
        *n = 0;
    }
    else if (((prefix_type == TK_ADJ_OP)   ||
              (prefix_type == TK_NEAR_OP)  ||
              (prefix_type == TK_WITH_OP)  ||
              (prefix_type == TK_AMONG_OP) ||
              (prefix_type == TK_ALONG_OP) ||
              (prefix_type == TK_SAME_OP)) && (has_nondigits == false))
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
        *type = TK_WILDCARD;
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
        while ((query[i] == '(') || (query[i] == ')') || (query[i] == '"') || (query[i] == '\''))
        {
            char *tmp = (char *) malloc(2 * sizeof(char));
            if (tmp == NULL)
            {
                exit(EXIT_FAILURE);
            }
            tmp[0] = query[i];
            tmp[1] = '\0';
            TokenType type = TK_ERROR;
            if (query[i] == '(')
            {
                type = TK_L_PAREN;
            }
            else if (query[i] == ')')
            {
                type = TK_R_PAREN;
            }
            else if ((query[i] == '"') || (query[i] == '\''))
            {
                type = TK_QUOTE;
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
            TokenType type = TK_ERROR;
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

bool
operator_token_type(TokenType type)
{
    if (type == TK_OR_OP    ||
        type == TK_AND_OP   ||
        type == TK_NOT_OP   ||
        type == TK_XOR_OP   ||
        type == TK_ADJ_OP   ||
        type == TK_NEAR_OP  ||
        type == TK_AMONG_OP ||
        type == TK_ALONG_OP ||
        type == TK_WITH_OP  ||
        type == TK_SAME_OP)
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int
count_errors_tokens(Token *list, bool print_errors)
{
    unsigned int n = 0;
    unsigned int n_quotes = 0;
    unsigned int n_l_parens = 0;
    unsigned int n_r_parens = 0;
    TokenType prev_type = TK_ERROR;
    Token *current = list;
    while (is_token(current) == true)
    {
        TokenType type = type_token(current);
        if (type == TK_ERROR)
        {
            n++;
            if (print_errors)
            {
                fprintf(stderr, "(%u) error in token '%s'\n", n, string_token(current));
            }
        }
        else if (type == TK_QUOTE)
        {
            n_quotes++;
        }
        else if (type == TK_L_PAREN)
        {
            n_l_parens++;
        }
        else if (type == TK_R_PAREN)
        {
            n_r_parens++;
        }
        else if (operator_token_type(prev_type) && operator_token_type(type))
        {
            n++;
            if (print_errors)
            {
                fprintf(stderr, "(%u) sequential operators at tokens '%s' and '%s'\n", n, string_token(prev_token(current)), string_token(current));
            }
        }
        else if (prev_type == TK_WILDCARD && type == TK_WILDCARD && (n_quotes % 2 == 0))
        {
            /* This should be impossible given that the code inserts default
             * operators, but I add it for completeness. */
            n++;
            if (print_errors)
            {
                fprintf(stderr, "(%u) sequential wildcards at tokens '%s' and '%s'\n", n, string_token(prev_token(current)), string_token(current));
            }
        }
        prev_type = type;
        current = next_token(current);
    }

    if (n_quotes % 2 != 0)
    {
        n++;
        if (print_errors)
        {
            fprintf(stderr, "(%u) unbalanced quotation marks present\n", n);
        }
    }
    if (n_l_parens != n_r_parens)
    {
        n++;
        if (print_errors)
        {
            fprintf(stderr, "(%u) unbalanced parentheses present\n", n);
        }
    }

    return n;
}

TokenType type_syntax_tree(SyntaxTree *tree)
{
    return tree->type;
}

int number_syntax_tree(SyntaxTree *tree)
{
    return tree->n;
}

char *string_syntax_tree(SyntaxTree *tree)
{
    return tree->string;
}

SyntaxTree *left_syntax_tree(SyntaxTree *tree)
{
    return tree->left;
}

SyntaxTree *right_syntax_tree(SyntaxTree *tree)
{
    return tree->right;
}

SyntaxTree *
insert_parent(TokenType type, int n, char *string, SyntaxTree *left, SyntaxTree *right)
{
    SyntaxTree *current = (SyntaxTree *) malloc(sizeof(SyntaxTree));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->type = type;
    current->n = n;
    current->string = string;
    current->left = left;
    current->right = right;
    return current;
}

void
print_syntax_tree(SyntaxTree *tree)
{
    if (tree != NULL)
    {
        TokenType type = type_syntax_tree(tree);
        if (type == TK_WILDCARD)
        {
            printf("%s", string_syntax_tree(tree));
        }
        else
        {
            printf("(");
            print_syntax_tree(left_syntax_tree(tree));
            printf(" %s", find_operator_prefix(type));
            if (number_syntax_tree(tree) != 0)
            {
                printf("%d", number_syntax_tree(tree));
            }
            printf(" ");
            print_syntax_tree(right_syntax_tree(tree));
            printf(")");
        }
    }
}

void
free_syntax_tree(SyntaxTree *tree)
{
    if (tree != NULL)
    {
        free(string_syntax_tree(tree));
        free_syntax_tree(left_syntax_tree(tree));
        free_syntax_tree(right_syntax_tree(tree));
    }
    free(tree);
}

SyntaxTree *
parse_query(Token **token)
{
    SyntaxTree *a = parse_expression_a(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type == TK_AND_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_a(token);
            a = insert_parent(TK_AND_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else if (type == TK_OR_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_a(token);
            a = insert_parent(TK_OR_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else if (type == TK_NOT_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_a(token);
            a = insert_parent(TK_NOT_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else if (type == TK_XOR_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_a(token);
            a = insert_parent(TK_XOR_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_expression_a(Token **token)
{
    SyntaxTree *a = parse_expression_b(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type == TK_SAME_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_b(token);
            a = insert_parent(TK_SAME_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_expression_b(Token **token)
{
    SyntaxTree *a = parse_expression_c(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type == TK_WITH_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_c(token);
            a = insert_parent(TK_WITH_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else if (type == TK_ALONG_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_c(token);
            a = insert_parent(TK_ALONG_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_expression_c(Token **token)
{
    SyntaxTree *a = parse_expression_d(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type == TK_AMONG_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_d(token);
            a = insert_parent(TK_AMONG_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_expression_d(Token **token)
{
    SyntaxTree *a = parse_expression_e(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type == TK_NEAR_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_expression_e(token);
            a = insert_parent(TK_NEAR_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_expression_e(Token **token)
{
    SyntaxTree *a = parse_atom(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type == TK_ADJ_OP)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_atom(token);
            a = insert_parent(TK_ADJ_OP, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_atom(Token **token)
{
    TokenType type = type_token(*token);
    if (type == TK_WILDCARD)
    {
        SyntaxTree *a = insert_parent(TK_WILDCARD, 0, string_token(*token), NULL, NULL);
        *token = next_token(*token);
        return a;
    }
    else if (type == TK_L_PAREN)
    {
        *token = next_token(*token);
        SyntaxTree *a = parse_query(token);
        assert(type_token(*token) == TK_R_PAREN);
        if (type_token(*token) == TK_R_PAREN)
        {
            *token = next_token(*token);
        }
        return a;
    }
    else if (type == TK_QUOTE)
    {
        *token = next_token(*token);
        SyntaxTree *a = parse_atom(token);
        while (type_token(*token) == TK_WILDCARD)
        {
            SyntaxTree *b = insert_parent(TK_WILDCARD, 0, string_token(*token), NULL, NULL);
            a = insert_parent(TK_ADJ_OP, 1, NULL, a, b);
            *token = next_token(*token);
        }
        assert(type_token(*token) == TK_QUOTE);
        if (type_token(*token) == TK_QUOTE)
        {
            *token = next_token(*token);
        }
        return a;
    }
    else
    {
        Token *current = *token;
        *token = next_token(*token);
        return insert_parent(TK_ERROR, number_token(current), string_token(current), NULL, NULL);
    }
}

Match *
eval_syntax_tree(SyntaxTree *tree, TrieNode *trie, bool *error_flag)
{
    Match *matches = NULL;
    TokenType type = type_syntax_tree(tree);
    if (type == TK_ERROR)
    {
        *error_flag = true;
        fprintf(stderr, "Fatal syntax error in token %s\n", string_syntax_tree(tree));
    }
    else if (type == TK_WILDCARD)
    {
        matches = wildcard_search(trie, string_syntax_tree(tree));
    }
    else
    {
        Match *left  = eval_syntax_tree(left_syntax_tree(tree),  trie, error_flag);
        Match *right = eval_syntax_tree(right_syntax_tree(tree), trie, error_flag);
        int n = number_syntax_tree(tree);
        if (*error_flag == false)
        {
            if (type == TK_OR_OP)
            {
                matches = op_or(left, right);
            }
            else if (type == TK_AND_OP)
            {
                matches = op_and(left, right);
            }
            else if (type == TK_NOT_OP)
            {
                matches = op_not(left, right);
            }
            else if (type == TK_XOR_OP)
            {
                matches = op_xor(left, right);
            }
            else if (type == TK_ADJ_OP)
            {
                matches = op_adj(left, right, n);
            }
            else if (type == TK_NEAR_OP)
            {
                matches = op_near(left, right, n);
            }
            else if (type == TK_AMONG_OP)
            {
                matches = op_among(left, right, n);
            }
            else if (type == TK_ALONG_OP)
            {
                matches = op_along(left, right, n);
            }
            else if (type == TK_WITH_OP)
            {
                matches = op_with(left, right, n);
            }
            else if (type == TK_SAME_OP)
            {
                matches = op_same(left, right, n);
            }
            else
            {
                *error_flag = true;
                fprintf(stderr, "Unidentified operator in token %s\n", string_syntax_tree(tree));
            }
        }
        free_matches(left);
        free_matches(right);
    }
    return matches;
}
