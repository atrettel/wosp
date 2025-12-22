// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "misc.h"
#include "operations.h"
#include "output.h"

#include <stdio.h>

void
insert_token(Token **list, TokenType type, int n, char *string, TokenType default_operator_type)
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
        const char *default_operator_string = find_operator_prefix(default_operator_type);
        char *tmp = (char *) allocmem((strlen(default_operator_string)+1), sizeof(char));
        snprintf(tmp, strlen(default_operator_string) + 1, "%s", default_operator_string);
        insert_token(list, default_operator_type, 0, tmp, default_operator_type);
    }
    else if ((proximity_operator_token_type(type) == true) && (prev_type == TK_NOT_OP))
    {
        TokenType new_type = TK_ERROR;
        if      (type == TK_ADJ_OP)   { new_type = TK_NOT_ADJ_OP;   }
        else if (type == TK_NEAR_OP)  { new_type = TK_NOT_NEAR_OP;  }
        else if (type == TK_AMONG_OP) { new_type = TK_NOT_AMONG_OP; }
        else if (type == TK_ALONG_OP) { new_type = TK_NOT_ALONG_OP; }
        else if (type == TK_WITH_OP)  { new_type = TK_NOT_WITH_OP;  }
        else if (type == TK_SAME_OP)  { new_type = TK_NOT_SAME_OP;  }
        type = new_type;
        char *prev_string = string_token(*list);
        char *tmp = (char *) allocmem((strlen(prev_string)+strlen(string)+1), sizeof(char));
        for (size_t i = 0; i < strlen(prev_string); i++)
        {
            tmp[i] = prev_string[i];
        }
        for (size_t i = 0; i < strlen(string); i++)
        {
            tmp[strlen(prev_string)+i] = string[i];
        }
        tmp[strlen(prev_string)+strlen(string)] = '\0';
        free(string);
        string = tmp;
        Token *prev_tkn = prev_token(*list);
        free((*list)->string);
        free(*list);
        *list = prev_tkn;
    }
    else if ((cumulative_quotes % 2 == 1) && (type != TK_QUOTE))
    {
        type = TK_WILDCARD;
    }
    Token *current = (Token *) allocmem(1, sizeof(Token));
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

static const char  *operator_prefixes[] = {    "or",     "and",     "not",     "xor",     "adj",     "near",     "among",     "along",     "with",     "same",      "notadj",      "notnear",      "notamong",      "notalong",      "notwith",      "notsame",     "icase",     "scase",     "lcase",     "ucase",     "tcase"};
static const TokenType operator_types[] = {TK_OR_OP, TK_AND_OP, TK_NOT_OP, TK_XOR_OP, TK_ADJ_OP, TK_NEAR_OP, TK_AMONG_OP, TK_ALONG_OP, TK_WITH_OP, TK_SAME_OP, TK_NOT_ADJ_OP, TK_NOT_NEAR_OP, TK_NOT_AMONG_OP, TK_NOT_ALONG_OP, TK_NOT_WITH_OP, TK_NOT_SAME_OP, TK_ICASE_OP, TK_SCASE_OP, TK_LCASE_OP, TK_UCASE_OP, TK_TCASE_OP};

static const char  *alias_prefixes[] = {  "around",    "notaround"};
static const TokenType alias_types[] = {TK_NEAR_OP, TK_NOT_NEAR_OP};

TokenType
find_operator_type(char *prefix)
{
    TokenType type = TK_ERROR;
    size_t n = sizeof(operator_types) / sizeof(operator_types[0]);
    for (size_t i = 0; i < n; i++)
    {
        if ((strncmp(prefix, operator_prefixes[i], strlen(prefix)) == 0)
         && (strlen(prefix) == strlen(operator_prefixes[i])))
        {
            type = operator_types[i];
            break;
        }
    }
    if (type == TK_ERROR)
    {
        n = sizeof(alias_types) / sizeof(alias_types[0]);
        for (size_t i = 0; i < n; i++)
        {
            if ((strncmp(prefix, alias_prefixes[i], strlen(prefix)) == 0)
             && (strlen(prefix) == strlen(alias_prefixes[i])))
            {
                type = alias_types[i];
                break;
            }
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
identify_token_type(char *data, TokenType *type, int *n)
{
    size_t len = strlen(data);
    size_t j = len; /* Where the numbers start, length of prefix */
    char *lcase = (char *) allocmem((len + 1), sizeof(char));
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
    char *prefix = (char *) allocmem((j + 1), sizeof(char));
    char *suffix = (char *) allocmem((k + 1), sizeof(char));
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
    if ((boolean_operator_token_type(prefix_type) == true) && (k == 0))
    {
        *type = prefix_type;
        *n = 0;
    }
    else if ((proximity_operator_token_type(prefix_type) == true) && (has_nondigits == false))
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
    else if ((search_operator_token_type(prefix_type) == true) && (has_nondigits == false))
    {
        *type = prefix_type;
        if (k == 0)
        {
            *n = 0;
        }
        else
        {
            *n = (int) m;
        }
        assert(*n >= 0);
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

TokenIterator
init_token_iterator(Token *token)
{
    TokenIterator iterator = {token};
    return iterator;
}

Token *
iterator_next_token(TokenIterator *iterator)
{
    Token *next = iterator->next;
    iterator->next = next_token(next);
    return next;
}

bool
iterator_has_next_token(TokenIterator iterator)
{
    return (iterator.next != NULL);
}

Token *
lex_query(char *query, TokenType default_operator_type)
{
    Token *tokens = NULL;
    size_t i = 0;
    size_t n = strlen(query);
    while (i < n)
    {
        while ((query[i] == '(') || (query[i] == ')') || (query[i] == '"') || (query[i] == '\''))
        {
            char *tmp = (char *) allocmem(2, sizeof(char));
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
            insert_token(&tokens, type, 0, tmp, default_operator_type);
            i++;
        }
        size_t len = 1;
        char *data = (char *) allocmem(len, sizeof(char));
        data[len - 1] = '\0';
        while ((isspace(query[i]) == false) && (query[i] != ')') && (query[i] != '"') && (query[i] != '\'') && (i < n))
        {
            len++;
            char *tmp = (char *) reallocmem(data, len);
            data = tmp;
            data[len - 2] = query[i];
            data[len - 1] = '\0';
            i++;
        }
        if (len > 1)
        {
            TokenType type = TK_ERROR;
            int n = -1;
            identify_token_type(data, &type, &n);
            insert_token(&tokens, type, n, data, default_operator_type);
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
    if (  boolean_operator_token_type(type) == true ||
        proximity_operator_token_type(type) == true ||
           search_operator_token_type(type) == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
boolean_operator_token_type(TokenType type)
{
    if (type == TK_OR_OP || type == TK_AND_OP || type == TK_NOT_OP || type == TK_XOR_OP)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
proximity_operator_token_type(TokenType type)
{
    if (type == TK_ADJ_OP   || type == TK_NOT_ADJ_OP   ||
        type == TK_NEAR_OP  || type == TK_NOT_NEAR_OP  ||
        type == TK_AMONG_OP || type == TK_NOT_AMONG_OP ||
        type == TK_ALONG_OP || type == TK_NOT_ALONG_OP ||
        type == TK_WITH_OP  || type == TK_NOT_WITH_OP  ||
        type == TK_SAME_OP  || type == TK_NOT_SAME_OP)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool
search_operator_token_type(TokenType type)
{
    if (type == TK_ICASE_OP || type == TK_SCASE_OP ||
        type == TK_LCASE_OP || type == TK_UCASE_OP ||
        type == TK_TCASE_OP)
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
    TokenIterator iterator = init_token_iterator(list);
    while (iterator_has_next_token(iterator) == true)
    {
        Token *current = iterator_next_token(&iterator);
        TokenType type = type_token(current);
        if (type == TK_ERROR)
        {
            n++;
            if (print_errors)
            {
                fprintf(stderr, "%s: (%u) Error in token '%s'\n", program_name, n, string_token(current));
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
        else if (operator_token_type(prev_type) &&
                      operator_token_type(type) &&
              (search_operator_token_type(type) == false))
        {
            n++;
            if (print_errors)
            {
                fprintf(stderr, "%s: (%u) Sequential operators at tokens '%s' and '%s'\n", program_name, n, string_token(prev_token(current)), string_token(current));
            }
        }
        else if (prev_type == TK_WILDCARD && type == TK_WILDCARD && (n_quotes % 2 == 0))
        {
            /* This should be impossible given that the code inserts default
             * operators, but I add it for completeness. */
            n++;
            if (print_errors)
            {
                fprintf(stderr, "%s: (%u) Sequential wildcards at tokens '%s' and '%s'\n", program_name, n, string_token(prev_token(current)), string_token(current));
            }
        }
        prev_type = type;
    }

    if (n_quotes % 2 != 0)
    {
        n++;
        if (print_errors)
        {
            fprintf(stderr, "%s: (%u) Unbalanced quotation marks present\n", program_name, n);
        }
    }
    if (n_l_parens != n_r_parens)
    {
        n++;
        if (print_errors)
        {
            fprintf(stderr, "%s: (%u) Unbalanced parentheses present\n", program_name, n);
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
    SyntaxTree *current = (SyntaxTree *) allocmem(1, sizeof(SyntaxTree));
    current->type = type;
    current->n = n;
    current->string = string;
    current->left = left;
    current->right = right;
    return current;
}

void
print_syntax_tree(FILE *stream, SyntaxTree *tree, bool terminal)
{
    if (tree != NULL)
    {
        TokenType type = type_syntax_tree(tree);
        if (type == TK_WILDCARD)
        {
            fprintf(stream, "%s", string_syntax_tree(tree));
        }
        else if (type == TK_ERROR)
        {
            fprintf(stream, "error");
        }
        else if (search_operator_token_type(type) == true)
        {
            fprintf(stream, "(");
            fprintf(stream, "%s", find_operator_prefix(type));
            if (number_syntax_tree(tree) != 0)
            {
                fprintf(stream, "%d", number_syntax_tree(tree));
            }
            fprintf(stream, " ");
            print_syntax_tree(stream, left_syntax_tree(tree), false);
            fprintf(stream, ")");
        }
        else
        {
            fprintf(stream, "(");
            print_syntax_tree(stream, left_syntax_tree(tree), false);
            fprintf(stream, " %s", find_operator_prefix(type));
            if (number_syntax_tree(tree) != 0)
            {
                fprintf(stream, "%d", number_syntax_tree(tree));
            }
            fprintf(stream, " ");
            print_syntax_tree(stream, right_syntax_tree(tree), false);
            fprintf(stream, ")");
        }
        if (terminal == true)
        {
            fprintf(stream, "\n");
        }
    }
}

void
free_syntax_tree(SyntaxTree *tree)
{
    if (tree != NULL)
    {
        free_syntax_tree(left_syntax_tree(tree));
        free_syntax_tree(right_syntax_tree(tree));
    }
    free(tree);
}

bool
type_in_list(TokenType type, TokenType *list, size_t n)
{
    bool result = false;
    for (size_t i = 0; i < n; i++)
    {
        if (list[i] == type)
        {
            result = true;
        }
    }
    return result;
}

SyntaxTree *
parse_types(Token ** token, TokenType *list, size_t n, SyntaxTree *parse_next(Token **))
{
    SyntaxTree *a = parse_next(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if (type_in_list(type, list, n) == true)
        {
            Token *op_token = *token;
            *token = next_token(*token);
            SyntaxTree *b = parse_next(token);
            a = insert_parent(type, number_token(op_token), string_token(op_token), a, b);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_query(Token **token)
{
    return parse_disjunction_op(token);
}

SyntaxTree *
parse_disjunction_op(Token **token)
{
    TokenType list[] = {TK_OR_OP, TK_XOR_OP};
    return parse_types(token, list, 2, parse_conjunction_op);
}

SyntaxTree *
parse_conjunction_op(Token **token)
{
    TokenType list[] = {TK_AND_OP};
    return parse_types(token, list, 1, parse_negation_op);
}

SyntaxTree *
parse_negation_op(Token **token)
{
    TokenType list[] = {TK_NOT_OP};
    return parse_types(token, list, 1, parse_paragraph_prox_op);
}

SyntaxTree *
parse_paragraph_prox_op(Token **token)
{
    TokenType list[] = {TK_SAME_OP, TK_NOT_SAME_OP};
    return parse_types(token, list, 2, parse_sentence_prox_op);
}

SyntaxTree *
parse_sentence_prox_op(Token **token)
{
    TokenType list[] = {TK_WITH_OP, TK_NOT_WITH_OP, TK_ALONG_OP, TK_NOT_ALONG_OP};
    return parse_types(token, list, 4, parse_clause_prox_op);
}

SyntaxTree *
parse_clause_prox_op(Token **token)
{
    TokenType list[] = {TK_AMONG_OP, TK_NOT_AMONG_OP};
    return parse_types(token, list, 2, parse_word_prox_op);
}

SyntaxTree *
parse_word_prox_op(Token **token)
{
    TokenType list[] = {TK_NEAR_OP, TK_NOT_NEAR_OP};
    return parse_types(token, list, 2, parse_adj_op);
}

SyntaxTree *
parse_adj_op(Token **token)
{
    bool in_quote = (type_token(*token) == TK_QUOTE) ? true : false;
    if (in_quote == true)
    {
        *token = next_token(*token);
    }
    SyntaxTree *a = parse_search_op(token);
    while (true)
    {
        TokenType type = type_token(*token);
        if ((type == TK_ADJ_OP || type == TK_NOT_ADJ_OP) && (in_quote == false))
        {
            Token *op_token = *token;
            *token = next_token(*token);
            if (type_token(*token) == TK_QUOTE)
            {
                in_quote = (in_quote == true) ? false : true;
                *token = next_token(*token);
            }
            SyntaxTree *b = parse_search_op(token);
            a = insert_parent(type, number_token(op_token), string_token(op_token), a, b);
        }
        else if (type == TK_WILDCARD && in_quote == true)
        {
            SyntaxTree *b = insert_parent(TK_WILDCARD, 0, string_token(*token), NULL, NULL);
            a = insert_parent(TK_ADJ_OP, 1, NULL, a, b);
            *token = next_token(*token);
        }
        else if (type == TK_QUOTE)
        {
            in_quote = (in_quote == true) ? false : true;
            *token = next_token(*token);
        }
        else
        {
            return a;
        }
    }
}

SyntaxTree *
parse_search_op(Token **token)
{
    TokenType type = type_token(*token);
    if (search_operator_token_type(type) == true)
    {
        Token *op_token = *token;
        *token = next_token(*token);
        SyntaxTree *a = parse_atom(token);
        return insert_parent(type, number_token(op_token), string_token(op_token), a, NULL);
    }
    else
    {
        return parse_atom(token);
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
    else
    {
        Token *current = *token;
        *token = next_token(*token);
        return insert_parent(TK_ERROR, number_token(current), string_token(current), NULL, NULL);
    }
}

Match *
eval_syntax_tree(SyntaxTree *tree, TrieNode *trie, CaseMode case_mode, unsigned int edit_dist, ProximityMode proximity_mode, bool *error_flag)
{
    Match *matches = NULL;
    TokenType type = type_syntax_tree(tree);
    if (type == TK_ERROR)
    {
        *error_flag = true;
        fprintf(stderr, "%s: Syntax error in token '%s'\n", program_name, string_syntax_tree(tree));
    }
    else if (type == TK_WILDCARD)
    {
        matches = wildcard_search(trie, string_syntax_tree(tree), case_mode, edit_dist);
    }
    else if (search_operator_token_type(type) == true)
    {
        CaseMode case_mode_tmp = case_mode;
        if      (type == TK_ICASE_OP) {case_mode_tmp = CM_INSENSITIVE;}
        else if (type == TK_SCASE_OP) {case_mode_tmp = CM_SENSITIVE;}
        else if (type == TK_LCASE_OP) {case_mode_tmp = CM_LOWERCASE;}
        else if (type == TK_UCASE_OP) {case_mode_tmp = CM_UPPERCASE;}
        else if (type == TK_TCASE_OP) {case_mode_tmp = CM_TITLE_CASE;}
        unsigned int edit_dist_tmp = (unsigned int) number_syntax_tree(tree);
        matches  = eval_syntax_tree(left_syntax_tree(tree), trie, case_mode_tmp, edit_dist_tmp, proximity_mode, error_flag);
    }
    else
    {
        Match *left  = eval_syntax_tree( left_syntax_tree(tree), trie, case_mode, edit_dist, proximity_mode, error_flag);
        Match *right = eval_syntax_tree(right_syntax_tree(tree), trie, case_mode, edit_dist, proximity_mode, error_flag);
        int n = number_syntax_tree(tree);
        if (*error_flag == false)
        {
            if      (type == TK_OR_OP)        {matches = op_or(       left, right   );}
            else if (type == TK_AND_OP)       {matches = op_and(      left, right   );}
            else if (type == TK_NOT_OP)       {matches = op_not(      left, right   );}
            else if (type == TK_XOR_OP)       {matches = op_xor(      left, right   );}
            else if (type == TK_ADJ_OP)       {matches = op_adj(      left, right, n, proximity_mode);}
            else if (type == TK_NEAR_OP)      {matches = op_near(     left, right, n, proximity_mode);}
            else if (type == TK_AMONG_OP)     {matches = op_among(    left, right, n, proximity_mode);}
            else if (type == TK_ALONG_OP)     {matches = op_along(    left, right, n, proximity_mode);}
            else if (type == TK_WITH_OP)      {matches = op_with(     left, right, n, proximity_mode);}
            else if (type == TK_SAME_OP)      {matches = op_same(     left, right, n, proximity_mode);}
            else if (type == TK_NOT_ADJ_OP)   {matches = op_not_adj(  left, right, n, proximity_mode);}
            else if (type == TK_NOT_NEAR_OP)  {matches = op_not_near( left, right, n, proximity_mode);}
            else if (type == TK_NOT_AMONG_OP) {matches = op_not_among(left, right, n, proximity_mode);}
            else if (type == TK_NOT_ALONG_OP) {matches = op_not_along(left, right, n, proximity_mode);}
            else if (type == TK_NOT_WITH_OP)  {matches = op_not_with( left, right, n, proximity_mode);}
            else if (type == TK_NOT_SAME_OP)  {matches = op_not_same( left, right, n, proximity_mode);}
            else
            {
                *error_flag = true;
                fprintf(stderr, "%s: Unidentified operator in token '%s'\n", program_name, string_syntax_tree(tree));
            }
        }
        free_matches(left);
        free_matches(right);
    }
    return matches;
}

void
interpret_query(char *query, TrieNode *trie, CaseMode case_mode, unsigned int edit_dist, ProximityMode proximity_mode, TokenType default_operator_type, OutputOptions options)
{
    Token *tokens = lex_query(query, default_operator_type);
    unsigned int n_errors = count_errors_tokens(tokens, true);
    if (n_errors == 0)
    {
        Token *current = tokens;
        SyntaxTree *tree = parse_query(&current);
        if (debug_syntax_tree == true)
        {
            print_syntax_tree(stdout, tree, true);
        }
        bool error_flag = false;
        Match *matches = eval_syntax_tree(tree, trie, case_mode, edit_dist, proximity_mode, &error_flag);
        if (error_flag == false)
        {
            if (type_output_options(options) == OT_DOCUMENTS)
            {
                print_documents_in_matches(matches, options);
            }
            else if (type_output_options(options) == OT_MATCHES)
            {
                print_matches(matches, options);
            }
            else if (type_output_options(options) == OT_EXCERPTS)
            {
                print_excerpts(matches, options);
            }
        }
        else
        {
            print_syntax_tree(stderr, tree, true);
            fprintf(stderr, "%s: One or more syntax errors found during evaluation\n", program_name);
        }
        free_syntax_tree(tree);
        free_matches(matches);
    }
    else
    {
        fprintf(stderr, "%s: One of more syntax errors found after tokenization\n", program_name);
    }
    free_tokens(tokens);
}
