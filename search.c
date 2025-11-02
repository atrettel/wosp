// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "search.h"
#include "words.h"

void
insert_match(Match **list, size_t n)
{
    assert(n > 0);
    Match *current = (Match *) malloc(sizeof(Match));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->n = n;
    current->words = (Word **) malloc(n * sizeof(Word *));
    if (current->words == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; i++)
    {
        current->words[i] = NULL;
    }
    current->next = *list;
    current->document = NULL;
    *list = current;
}

void
set_match(Match *match, size_t i, Word *word)
{
    assert(i >= 0);
    assert(i < number_of_words_in_match(match));
    if (match->document == NULL)
    {
        match->document = document_word(word);
    }
    assert(document_match(match) == document_word(word));
    match->words[i] = word;
}

/* This only copies current match.  It does not go down the list. */
void
append_match(Match *current, Match **dest)
{
    size_t n = number_of_words_in_match(current);
    insert_match(dest, n);
    for (size_t i = 0; i < n; i++)
    {
        set_match(*dest, i, word_match(current, i));
    }
}

size_t
number_of_words_in_match(Match *match)
{
    return match->n;
}

unsigned int
length_of_match_list(Match *match)
{
    Match *current = match;
    unsigned int n = 0;
    while (is_match(current) == true)
    {
        n++;
        current = next_match(current);
    }
    return n;
}

Word *
word_match(Match *match, size_t i)
{
    assert(i >= 0);
    assert(i < number_of_words_in_match(match));
    return match->words[i];
}

Word *
document_match(Match *match)
{
    assert(match->document != NULL);
    return match->document;
}

DocumentNode *
document_list_match_list(Match *match)
{
    DocumentNode *list = NULL;
    Match *current = match;
    while (is_match(current) == true)
    {
        insert_document(&list, document_match(current));
        current = next_match(current);
    }
    return list;
}

bool
is_match(Match * match)
{
    if (match == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

Match *
next_match(Match *match)
{
    if (is_match(match) == true)
    {
        return match->next;
    }
    else
    {
        return NULL;
    }
}

Word *
start_word_match(Match *match)
{
    Word *start = word_match(match, 0);
    size_t n = number_of_words_in_match(match);
    for (size_t i = 1; i < n; i++)
    {
        Word *word = word_match(match, i);
        if (position_word(word) < position_word(start))
        {
            start = word;
        }
    }
    return start;
}

Word *
end_word_match(Match *match)
{
    Word *end = word_match(match, 0);
    size_t n = number_of_words_in_match(match);
    for (size_t i = 1; i < n; i++)
    {
        Word *word = word_match(match, i);
        if (position_word(word) > position_word(end))
        {
            end = word;
        }
    }
    return end;
}

unsigned int
start_position_match(Match *match)
{
    return position_word(start_word_match(match));
}

unsigned int
end_position_match(Match *match)
{
    return position_word(end_word_match(match));
}

/* number_of_words_in_match returns the number of searched words.  This
 * function returns the total number of words, including those that the user
 * has not searched for in particular. */
unsigned int
width_match(Match *match)
{
    return end_position_match(match) - start_position_match(match) + 1;
}

void
concatenate_matches(Match *src, Match **dest)
{
    Match *current = src;
    while (is_match(current) == true)
    {
        append_match(current, dest);
        current = next_match(current);
    }
}

void
free_matches(Match *list)
{
    Match *current = list;
    while (is_match(current) == true)
    {
        Match *next = next_match(current);
        free(current->words);
        free(current);
        current = next;
    }
}

void
init_trie(TrieNode **trie)
{
    TrieNode *current = (TrieNode *) malloc(sizeof(TrieNode));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->key = '\0';
    current->edges = NULL;
    current->match = NULL;
    *trie = current;
}

void
insert_trie(TrieNode *trie, Word *word, size_t i)
{
    char *reduced = reduced_word(word);
    char key = reduced[i];

    /* If we are using the entire word, stop and add the match to the list. */
    if (i == strlen(reduced))
    {
        insert_match(&(trie->match), 1);
        set_match(trie->match, 0, word);
    }
    else
    {
        /* If the key is in the current list of edges, recurse down that edge. */
        TrieEdge *edge = trie->edges;
        while (edge != NULL)
        {
            if (edge->node->key == key)
            {
                insert_trie(edge->node, word, i+1);
                break;
            }
            else
            {
                edge = edge->next;
            }
        }
        if (edge == NULL)
        {
            /* The key is not in the current list of edges.  Add it. */
            TrieEdge *current = (TrieEdge *) malloc(sizeof(TrieEdge));
            if (current == NULL)
            {
                exit(EXIT_FAILURE);
            }
            current->next = trie->edges;
            current->node = (TrieNode *) malloc(sizeof(TrieNode));
            if (current->node == NULL)
            {
                exit(EXIT_FAILURE);
            }
            current->node->key = key;
            current->node->edges = NULL;
            current->node->match = NULL;
            insert_trie(current->node, word, i+1);
            trie->edges = current;
        }
    }
}

bool
has_word_trie(TrieNode *trie, char *reduced)
{
    Match *match = NULL;
    backtrack_trie(trie, reduced, 0, &match);
    bool result = false;
    if (is_match(match) == false)
    {
        result = false;
    }
    else
    {
        result = true;
    }
    free_matches(match);
    return result;
}

void
backtrack_trie(TrieNode *trie, char *reduced, size_t i, Match **match)
{
    char key = reduced[i];
    if (i == strlen(reduced))
    {
        concatenate_matches(trie->match, match);
    }
    else
    {
        TrieEdge *edge = trie->edges;
        while (edge != NULL)
        {
            if ((key == wildcard_character) || (edge->node->key == key))
            {
                backtrack_trie(edge->node, reduced, i+1, match);
            }
            edge = edge->next;
        }
    }
}

void
expand_word(TrieNode *trie, char *original, size_t i, Match **match, CaseMode case_mode, unsigned int edit_dist)
{
    char c = original[i];
    if (i == strlen(original))
    {
        char *reduced = reduce_word(original, WO_QUERY);
        backtrack_trie(trie, reduced, 0, match);
        free(reduced);
    }
    else
    {
        if (is_truncation_character(c) == true)
        {
            size_t m = i + 1; /* End position of digits */
            size_t n = 0; /* Maximum number of wildcard characters */
            bool has_digits = false;
            while ((m != strlen(original)) && (isdigit(original[m])))
            {
                m++;
            }
            if ((m - i - 1) == 0)
            {
                has_digits = false;
                n = height_trie(trie) - 1;
            }
            else
            {
                has_digits = true;
                char *tmp = (char *) malloc((m - i) * sizeof(char));
                if (tmp == NULL)
                {
                    exit(EXIT_FAILURE);
                }
                for (size_t k = 0; k < (m - i - 1); k++)
                {
                    tmp[k] = original[i+k+1];
                }
                tmp[m-i-1] = '\0';
                char *endptr;
                n = (size_t) strtol(tmp, &endptr, 10);
                free(tmp);
            }
            for (size_t j = 0; j <= n; j++)
            {
                size_t len = j + strlen(original) - ((has_digits == true) ?  1 : 0);
                char *modified = (char *) malloc(len * sizeof(char));
                if (modified == NULL)
                {
                    exit(EXIT_FAILURE);
                }
                for (size_t k = 0; k < i; k++)
                {
                    modified[k] = original[k];
                }
                for (size_t k = 0; k < j; k++)
                {
                    modified[i+k] = wildcard_character;
                }
                for (size_t k = 0; k < (len - i - j); k++)
                {
                    modified[i+j+k] = original[k+m];
                }
                modified[len-1] = '\0';
                expand_word(trie, modified, i, match, case_mode, edit_dist);
                free(modified);
            }
        }
        else
        {
            if (!isalpha(c) || (case_mode == CM_SENSITIVE))
            {
                expand_word(trie, original, i+1, match, case_mode, edit_dist);
            }
            else
            {
                size_t len = strlen(original) + 1;
                char *modified = (char *) malloc(len * sizeof(char));
                if (modified == NULL)
                {
                    exit(EXIT_FAILURE);
                }
                snprintf(modified, len, "%s", original);
                if ((case_mode == CM_INSENSITIVE) || (case_mode == CM_LOWERCASE) || ((case_mode == CM_TITLE_CASE) && (i > 0)))
                {
                    modified[i] = tolower(c);
                    expand_word(trie, modified, i+1, match, case_mode, edit_dist);
                }
                if ((case_mode == CM_INSENSITIVE) || (case_mode == CM_UPPERCASE) || ((case_mode == CM_TITLE_CASE) && (i == 0)))
                {
                    modified[i] = toupper(c);
                    expand_word(trie, modified, i+1, match, case_mode, edit_dist);
                }
                free(modified);
            }
            if (edit_dist > 0)
            {
                size_t len;
                char *modified;
                /* Insertion */
                len = strlen(original) + 2;
                modified = (char *) malloc(len * sizeof(char));
                for (size_t j = 0; j < i; j++)
                {
                    modified[j] = original[j];
                }
                modified[i] = wildcard_character;
                for (size_t j = i+1; j < len; j++)
                {
                    modified[j] = original[j-1];
                }
                modified[len-1] = '\0';
                expand_word(trie, modified, i+2, match, case_mode, edit_dist-1);
                free(modified);
                /* Final insertion */
                if (i + 1 == strlen(original))
                {
                    len = strlen(original) + 2;
                    modified = (char *) malloc(len * sizeof(char));
                    for (size_t j = 0; j < strlen(original); j++)
                    {
                        modified[j] = original[j];
                    }
                    modified[len-2] = wildcard_character;
                    modified[len-1] = '\0';
                    expand_word(trie, modified, i+2, match, case_mode, edit_dist-1);
                    free(modified);
                }
                /* Deletion */
                if (strlen(original) > 1)
                {
                    len = strlen(original);
                    modified = (char *) malloc(len * sizeof(char));
                    for (size_t j = 0; j < i; j++)
                    {
                        modified[j] = original[j];
                    }
                    for (size_t j = i; j < len; j++)
                    {
                        modified[j] = original[j+1];
                    }
                    modified[len-1] = '\0';
                    expand_word(trie, modified, i, match, case_mode, edit_dist-1);
                    free(modified);
                }
                /* Substitution */
                len = strlen(original) + 1;
                modified = (char *) malloc(len * sizeof(char));
                if (modified == NULL)
                {
                    exit(EXIT_FAILURE);
                }
                snprintf(modified, len, "%s", original);
                modified[i] = wildcard_character;
                expand_word(trie, modified, i+1, match, case_mode, edit_dist-1);
                free(modified);
                /* Transposition */
                if (i + 1 < strlen(original))
                {
                    len = strlen(original) + 1;
                    modified = (char *) malloc(len * sizeof(char));
                    if (modified == NULL)
                    {
                        exit(EXIT_FAILURE);
                    }
                    snprintf(modified, len, "%s", original);
                    modified[i]   = original[i+1];
                    modified[i+1] = original[i];
                    expand_word(trie, modified, i+1, match, case_mode, edit_dist-1);
                    free(modified);
                }
            }
        }
    }
}

size_t
height_trie(TrieNode *trie)
{
    if (trie == NULL)
    {
        return 0;
    }
    else
    {
        size_t max_edge_height = 0;
        TrieEdge *edge = trie->edges;
        while (edge != NULL)
        {
            size_t edge_height = height_trie(edge->node);
            if (edge_height > max_edge_height)
            {
                max_edge_height = edge_height;
            }
            edge = edge->next;
        }
        return max_edge_height + 1;
    }
}

void
free_trie(TrieNode *trie)
{
    if (trie != NULL)
    {
        TrieEdge *edge = trie->edges;
        while (edge != NULL)
        {
            TrieEdge *next = edge->next;
            free_trie(edge->node);
            free(edge);
            edge = next;
        }
        free_matches(trie->match);
        free(trie);
    }
}

void
insert_document(DocumentNode **list, Word *document)
{
    if (has_document(*list, document) == false)
    {
        DocumentNode *current = malloc(sizeof(DocumentNode));
        if (current == NULL)
        {
            exit(EXIT_FAILURE);
        }
        current->document = document;
        current->next = *list;
        *list = current;
    }
}

Word *
document_document(DocumentNode *list)
{
    if (is_document(list) == true)
    {
        return list->document;
    }
    else
    {
        return NULL;
    }
}

DocumentNode *
next_document(DocumentNode *list)
{
    if (is_document(list) == true)
    {
        return list->next;
    }
    else
    {
        return NULL;
    }
}

bool
has_document(DocumentNode *list, Word *document)
{
    DocumentNode *current = list;
    while (is_document(current) == true)
    {
        if (current->document == document)
        {
            return true;
        }
        current = next_document(current);
    }
    return false;
}

bool
is_document(DocumentNode *list)
{
    if (list == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

char *
filename_document(DocumentNode *document)
{
    return filename_word(document->document);
}

void
free_document_list(DocumentNode *list)
{
    DocumentNode *current = list;
    while (is_document(current) == true)
    {
        DocumentNode *next = next_document(current);
        free(current);
        current = next;
    }
}

Match *
wildcard_search(TrieNode *trie, char *original, CaseMode case_mode, unsigned int edit_dist)
{
    Match *match = NULL;
    expand_word(trie, original, 0, &match, case_mode, edit_dist);
    return match;
}

Match *
proximity_search(Match *first_match, Match *second_match, LanguageElement element, int start, int end, bool inclusive_proximity)
{
    Match *match = NULL;
    Match *outer_match = first_match;
    while (is_match(outer_match) == true)
    {
        Word *outer_start_word = advance_word(start_word_match(outer_match), element, start);
        Word   *outer_end_word = advance_word(  end_word_match(outer_match), element,   end);
        unsigned long outer_start = position_word(outer_start_word);
        unsigned long   outer_end =   position_word(outer_end_word);

        /* Clauses, lines, sentences, paragraphs, and pages return the start of
         * the next element.  Decrement to include only the desired element,
         * but only when it is not the end of the document. */
        if ((element == LE_CLAUSE    ||
             element == LE_LINE      ||
             element == LE_SENTENCE  ||
             element == LE_PARAGRAPH ||
             element == LE_PAGE) && (field_has_next_word(outer_end_word) == true))
        {
            outer_end--;
        }

        Match *inner_match = second_match;
        while (is_match(inner_match) == true)
        {
            if (document_match(outer_match) == document_match(inner_match))
            {
                unsigned long inner_start = position_word(start_word_match(inner_match));
                unsigned long   inner_end = position_word(  end_word_match(inner_match));
                if (((inner_start >= outer_start) && (inner_end <= outer_end)   && (inclusive_proximity == false)) ||
                    ((inner_start <= outer_end)   && (inner_end >= outer_start) && (inclusive_proximity == true )))
                {
                    /* We have a match.  Let's add it. */
                    size_t n_outer = number_of_words_in_match(outer_match);
                    size_t n_inner = number_of_words_in_match(inner_match);
                    insert_match(&match, n_outer + n_inner);
                    for (size_t i = 0; i < n_outer; i++)
                    {
                        set_match(match, i, word_match(outer_match, i));
                    }
                    for (size_t i = 0; i < n_inner; i++)
                    {
                        set_match(match, n_outer + i, word_match(inner_match, i));
                    }
                }
            }
            inner_match = next_match(inner_match);
        }
        outer_match = next_match(outer_match);
    }
    return match;
}
