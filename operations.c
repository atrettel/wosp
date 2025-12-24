// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "operations.h"
#include "search.h"
#include "words.h"

static Match *
op_boolean(Match *first_match, Match *second_match, bool condition(DocumentNode *, DocumentNode *, Word *))
{
    Match *match = NULL;
    DocumentNode *first_documents = document_list_match_list(first_match);
    DocumentNode *second_documents = document_list_match_list(second_match);
    bool processed_first = false;
    MatchIterator iterator = init_match_iterator(first_match);
    while (iterator_has_next_match(iterator) == true)
    {
        Match *current_match = iterator_next_match(&iterator);
        if (condition(first_documents, second_documents, document_match(current_match)) == true)
        {
            append_match(current_match, &match);
        }
        if ((iterator_has_next_match(iterator) == false) && (processed_first == false))
        {
            processed_first = true;
            iterator = init_match_iterator(second_match);
        }
    }
    free_document_list(first_documents);
    free_document_list(second_documents);
    return match;
}

Match *
op_or(Match *first_match, Match *second_match)
{
    Match *match = NULL;
    concatenate_matches(first_match, &match);
    concatenate_matches(second_match, &match);
    return match;
}

static bool
cond_and(DocumentNode *first, DocumentNode *second, Word *document)
{
    if ((has_document(first, document) == true) && (has_document(second, document) == true))
    {
        return true;
    }
    else
    {
        return false;
    }
}

Match *
op_and(Match *first_match, Match *second_match)
{
    return op_boolean(first_match, second_match, cond_and);
}

static bool
cond_not(DocumentNode *first, DocumentNode *second, Word *document)
{
    if ((has_document(first, document) == true) && (has_document(second, document) == false))
    {
        return true;
    }
    else
    {
        return false;
    }
}

Match *
op_not(Match *first_match, Match *second_match)
{
    return op_boolean(first_match, second_match, cond_not);
}

static bool
cond_xor(DocumentNode *first, DocumentNode *second, Word *document)
{
    if ((has_document(first, document) == true) && (has_document(second, document) == false))
    {
        return true;
    }
    else if ((has_document(first, document) == false) && (has_document(second, document) == true))
    {
        return true;
    }
    else
    {
        return false;
    }
}

Match *
op_xor(Match *first_match, Match *second_match)
{
    return op_boolean(first_match, second_match, cond_xor);
}

Match *
op_adj(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_WORD, 1, n, proximity_mode);
}

Match *
op_near(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_WORD, -n, +n, proximity_mode);
}

Match *
op_among(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_CLAUSE, -n, +n, proximity_mode);
}

Match *
op_along(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_LINE, -n, +n, proximity_mode);
}

Match *
op_with(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_SENTENCE, -n, +n, proximity_mode);
}

Match *
op_same(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_PARAGRAPH, -n, +n, proximity_mode);
}

static Match *
op_not_prox(Match *first_match, Match *second_match, int n, Match *op_prox(Match *, Match *, int, ProximityMode), ProximityMode proximity_mode)
{
    Match *union_match = op_or(first_match, second_match);
    Match *prox_match = op_prox(first_match, second_match, n, proximity_mode);
    Match *match = NULL;
    MatchIterator outer_iterator = init_match_iterator(union_match);
    while (iterator_has_next_match(outer_iterator) == true)
    {
        Match *outer_match = iterator_next_match(&outer_iterator);
        size_t n_outer = number_of_words_in_match(outer_match);
        bool found = false;
        MatchIterator inner_iterator = init_match_iterator(prox_match);
        while (iterator_has_next_match(inner_iterator) == true)
        {
            Match *inner_match = iterator_next_match(&inner_iterator);
            size_t n_inner = number_of_words_in_match(inner_match);
            for (size_t i = 0; i < n_outer; i++)
            {
                for (size_t j = 0; j < n_inner; j++)
                {
                    if (word_match(outer_match, i) == word_match(inner_match, j))
                    {
                        found = true;
                    }
                }
            }
        }
        if (found == false)
        {
            insert_match(&match, n_outer);
            for (size_t i = 0; i < n_outer; i++)
            {
                set_match(match, i, word_match(outer_match, i));
            }
        }
    }
    free_matches(union_match);
    free_matches(prox_match);
    return match;
}

Match *
op_not_adj(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    return op_not_prox(first_match, second_match, n, op_adj, proximity_mode);
}

Match *
op_not_near(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    return op_not_prox(first_match, second_match, n, op_near, proximity_mode);
}

Match *
op_not_among(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    return op_not_prox(first_match, second_match, n, op_among, proximity_mode);
}

Match *
op_not_along(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    return op_not_prox(first_match, second_match, n, op_along, proximity_mode);
}

Match *
op_not_with(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    return op_not_prox(first_match, second_match, n, op_with, proximity_mode);
}

Match *
op_not_same(Match *first_match, Match *second_match, int n, ProximityMode proximity_mode)
{
    return op_not_prox(first_match, second_match, n, op_same, proximity_mode);
}
