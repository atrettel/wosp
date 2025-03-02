/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>

#include "operations.h"
#include "search.h"
#include "words.h"

Match *
op_or(Match *first_match, Match *second_match)
{
    Match *match = NULL;
    concatenate_matches(first_match, &match);
    concatenate_matches(second_match, &match);
    return match;
}

Match *
op_and(Match *first_match, Match *second_match)
{
    Match *match = NULL;
    if ((first_match != NULL) && (second_match != NULL))
    {
        concatenate_matches(first_match, &match);
        concatenate_matches(second_match, &match);
    }
    return match;
}

Match *
op_not(Match *first_match, Match *second_match)
{
    Match *match = NULL;
    if ((first_match != NULL) && (second_match == NULL))
    {
        concatenate_matches(first_match, &match);
    }
    return match;
}

Match *
op_xor(Match *first_match, Match *second_match)
{
    Match *match = NULL;
    if ((first_match != NULL) && (second_match == NULL))
    {
        concatenate_matches(first_match, &match);
    }
    else if ((first_match == NULL) && (second_match != NULL))
    {
        concatenate_matches(second_match, &match);
    }
    return match;
}

Match *
op_near(Match *first_match, Match *second_match, int n)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, WORD, -n, +n);
}

Match *
op_adj(Match *first_match, Match *second_match, int n)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, WORD, 1, n);
}

Match *
op_with(Match *first_match, Match *second_match, int n)
{
    assert(n > 0);
    return proximity_search(first_match, second_match, SENTENCE, -n, +n);
}

