/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "search.h"
#include "words.h"

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
start_word_match(Match *match)
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
end_word_match(Match *match)
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
print_matches(Match *match)
{
    printf("%u matches\n", length_of_match_list(match));
    Match *current_match = match;
    while (current_match != NULL)
    {
        InputWord *start_word = advance_word(start_word_match(current_match), print_element, -print_width);
        InputWord *end_word   = advance_word(  end_word_match(current_match), print_element, +print_width);
        printf("%lu:", line_word(start_word));
        InputWord *current_word = start_word;
        while (current_word != end_word)
        {
            printf(" %s", original_word(current_word));
            current_word = next_word(current_word);
        }
        printf("\n");
        current_match = next_match(current_match);
    }
}

Match *
copy_matches(Match *match)
{
    Match *copy = NULL;
    Match *current = match;
    while (current != NULL)
    {
        size_t n = number_of_words_in_match(current);
        append_match(&copy, n);
        for (size_t i = 0; i < n; i++)
        {
            set_match(copy, i, word_match(current, i));
        }
        current = next_match(current);
    }
    return copy;
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

/* match_trie is the primitive operation that returns the same pointer as in
 * the trie data structure.  word_search is a wrapper function that makes a
 * copy of the matches and applies case sensitivity if needed. */
Match *
match_trie(TrieNode *trie, char *reduced, size_t i)
{
    char key = reduced[i];

    Match *match = NULL;
    if (i == strlen(reduced))
    {
        match = trie->match;
    }
    else
    {
        TrieEdge *edge = trie->edges;
        while (edge != NULL)
        {
            if (edge->node->key == key)
            {
                match = match_trie(edge->node, reduced, i+1);
                break;
            }
            else
            {
                edge = edge->next;
            }
        }
    }
    return match;
}

bool
has_word_trie(TrieNode *trie, char *reduced)
{
    Match *match = match_trie(trie, reduced, 0);
    if (match == NULL)
    {
        return false;
    }
    else
    {
        return true;
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

Match *
word_search(TrieNode *trie, char *original)
{
    char *reduced = reduce_word(original);
    Match *match = match_trie(trie, reduced, 0);
    Match *copy = copy_matches(match);
    free(reduced);
    return copy;
}
