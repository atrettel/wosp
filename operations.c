/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "operations.h"
#include "search.h"
#include "words.h"

Match *
op_boolean(Match *first_match, Match *second_match, bool condition(DocumentNode *, DocumentNode *, Word *))
{
    Match *match = NULL;
    DocumentNode *first_documents = document_list_match_list(first_match);
    DocumentNode *second_documents = document_list_match_list(second_match);
    bool processed_first = false;
    Match *current_match = first_match;
    while (is_match(current_match) == true)
    {
        if (condition(first_documents, second_documents, document_match(current_match)) == true)
        {
            append_match(current_match, &match);
        }
        current_match = next_match(current_match);
        if ((is_match(current_match) == false) && (processed_first == false))
        {
            processed_first = true;
            current_match = second_match;
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

bool
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

bool
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

bool
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
op_adj(Match *first_match, Match *second_match, int n)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_WORD, 1, n);
}

Match *
op_near(Match *first_match, Match *second_match, int n)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_WORD, -n, +n);
}

Match *
op_with(Match *first_match, Match *second_match, int n)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, LE_SENTENCE, -n, +n);
}

