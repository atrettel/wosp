/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "search.h"

void
append_match(Match **list, size_t n)
{
    Match *current = (Match *) malloc(sizeof(Match));
    if (current == NULL)
    {
        exit(EXIT_FAILURE);
    }
    current->n = n;
    current->words = (InputWord **) malloc(n * sizeof(InputWord *));
    if (current->words == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; i++)
    {
        current->words[i] = NULL;
    }
    current->next = *list;
    *list = current;
}

void
set_match(Match *match, size_t i, InputWord *word)
{
    assert(i >= 0);
    assert(i < number_of_words_in_match(match));
    match->words[i] = word;
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
    while (current != NULL)
    {
        n++;
        current = next_match(current);
    }
    return n;
}

InputWord *
word_match(Match *match, size_t i)
{
    assert(i >= 0);
    assert(i < number_of_words_in_match(match));
    return match->words[i];
}

Match *
next_match(Match *match)
{
    if (match == NULL)
    {
        return NULL;
    }
    else
    {
        return match->next;
    }
}

InputWord *
start_match(Match *match)
{
    InputWord *start = word_match(match, 0);
    size_t n = number_of_words_in_match(match);
    for (size_t i = 1; i < n; i++)
    {
        InputWord *word = word_match(match, i);
        if (position_word(word) < position_word(start))
        {
            start = word;
        }
    }
    return start;
}

InputWord *
end_match(Match *match)
{
    InputWord *end = word_match(match, 0);
    size_t n = number_of_words_in_match(match);
    for (size_t i = 1; i < n; i++)
    {
        InputWord *word = word_match(match, i);
        if (position_word(word) > position_word(end))
        {
            end = word;
        }
    }
    return end;
}

void
free_matches(Match *list)
{
    Match *current = list;
    while (current != NULL)
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
insert_trie(TrieNode *trie, InputWord *word, size_t i)
{
    char *reduced = reduced_word(word);
    char key = reduced[i];

    /* If we are using the entire word, stop and add the match to the list. */
    if (i == strlen(reduced))
    {
        append_match(&(trie->match), 1);
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
