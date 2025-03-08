/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdlib.h>

#include "operations.h"
#include "search.h"
#include "words.h"

size_t
count_matches_by_document(Match *first_match, Match *second_match,
                          InputWord ***documents,
                          unsigned int **first_counts, unsigned int **second_counts)
{
    size_t n_documents = 0;
    bool processed_first = false;
    Match *current = first_match;
    while (current != NULL)
    {
        bool found_document = false;
        for (size_t i = 0; i < n_documents; i++)
        {
            if (document_match(current) == (*documents)[i])
            {
                if (processed_first == false)
                {
                    (*first_counts)[i] = (*first_counts)[i] + 1;
                }
                else
                {
                    (*second_counts)[i] = (*second_counts)[i] + 1;
                }
                found_document = true;
                break;
            }
        }
        if (found_document == false)
        {
            n_documents++;
            if (n_documents == 1)
            {
                *documents = (InputWord **) malloc(n_documents * sizeof(InputWord *));
                *first_counts = (unsigned int *) malloc(n_documents * sizeof(unsigned int));
                *second_counts = (unsigned int *) malloc(n_documents * sizeof(unsigned int));
            }
            else
            {
                *documents = (InputWord **) realloc(*documents, n_documents);
                *first_counts = (unsigned int *) realloc(*first_counts, n_documents);
                *second_counts = (unsigned int *) realloc(*second_counts, n_documents);
            }
            if ((documents == NULL) || (*first_counts == NULL) || (*second_counts == NULL))
            {
                exit(EXIT_FAILURE);
            }
            (*documents)[n_documents-1] = document_match(current);
            (*first_counts)[n_documents-1] = 0;
            (*second_counts)[n_documents-1] = 0;
            if (processed_first == false)
            {
                (*first_counts)[n_documents-1] = 1;
            }
            else
            {
                (*second_counts)[n_documents-1] = 1;
            }
        }
        current = next_match(current);
        if ((current == NULL) && (processed_first == false))
        {
            processed_first = true;
            current = second_match;
        }
    }
    return n_documents;
}

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

    InputWord **documents = NULL;
    unsigned int *first_counts = NULL;
    unsigned int *second_counts = NULL;
    size_t n_documents = count_matches_by_document(first_match, second_match,
        &documents, &first_counts, &second_counts);

    bool processed_first = false;
    Match *current = first_match;
    while (current != NULL)
    {
        InputWord *document = document_match(current);
        for (size_t i = 0; i < n_documents; i++)
        {
            if ((document == documents[i]) &&  (first_counts[i] > 0)
                                           && (second_counts[i] > 0))
            {
                copy_match(current, &match);
                break;
            }
        }

        current = next_match(current);
        if ((current == NULL) && (processed_first == false))
        {
            processed_first = true;
            current = second_match;
        }
    }

    free(documents);
    free(first_counts);
    free(second_counts);

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

