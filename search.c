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
    *list = current;
}

void
set_match(Match *match, size_t i, Word *word)
{
    assert(i >= 0);
    assert(i < number_of_words_in_match(match));
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
    while (current != NULL)
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
    assert(match->words[0] != NULL);
    return first_word(match->words[0]);
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
print_matches(Match *match)
{
    printf("%u matches\n", length_of_match_list(match));
    Match *current_match = match;
    while (current_match != NULL)
    {
        Word *start_word = advance_word(start_word_match(current_match), print_element, -print_width);
        Word *end_word   = advance_word(  end_word_match(current_match), print_element, +print_width);
        if (next_word(end_word) == NULL)
        {
            end_word = NULL;
        }
        printf("%lu:", line_word(start_word));
        Word *current_word = start_word;
        while (current_word != end_word)
        {
            printf(" %s", original_word(current_word));
            current_word = next_word(current_word);
        }
        printf("\n");
        current_match = next_match(current_match);
    }
}

void
concatenate_matches(Match *src, Match **dest)
{
    Match *current = src;
    while (current != NULL)
    {
        append_match(current, dest);
        current = next_match(current);
    }
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
    if (match == NULL)
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
expand_word(TrieNode *trie, char *original, size_t i, Match **match)
{
    char c = original[i];
    if (i == strlen(original))
    {
        char *reduced = reduce_word(original, QUERY);
        backtrack_trie(trie, reduced, 0, match);
        free(reduced);
    }
    else
    {
        if (c == extended_wildcard_character)
        {
            assert((i + 1 < strlen(original)));
            size_t n = (size_t) (original[i+1] - '0');
            assert(n > 0);
            assert(n < 10);
            for (size_t j = 0; j <= n; j++)
            {
                size_t len = j + strlen(original) - 1;
                char *modified = malloc(len * sizeof(char));
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
                    modified[i+j+k] = original[i+k+2];
                }
                modified[len-1] = '\0';
                expand_word(trie, modified, i, match);
                free(modified);
            }
        }
        else
        {
            expand_word(trie, original, i+1, match);
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

Match *
wildcard_search(TrieNode *trie, char *original)
{
    Match *match = NULL;
    expand_word(trie, original, 0, &match);
    return match;
}

Match *
proximity_search(Match *first_match, Match *second_match, LanguageElement element, int start, int end)
{
    Match *match = NULL;
    Match *outer_match = first_match;
    while (outer_match != NULL)
    {
        Word *outer_start_word = advance_word(start_word_match(outer_match), element, start);
        Word   *outer_end_word = advance_word(  end_word_match(outer_match), element,   end);
        unsigned long outer_start = position_word(outer_start_word);
        unsigned long   outer_end =   position_word(outer_end_word);

        /* Clauses and sentences return the start of the next element.
         * Decrement to include only the desired element. */
        if ((element == CLAUSE || element == SENTENCE) && (next_word(outer_end_word) != NULL))
        {
            outer_end--;
        }

        Match *inner_match = second_match;
        while (inner_match != NULL)
        {
            if (document_match(outer_match) == document_match(inner_match))
            {
                unsigned long inner_start = position_word(start_word_match(inner_match));
                unsigned long   inner_end = position_word(  end_word_match(inner_match));
                if ((inner_start >= outer_start) && (inner_end <= outer_end))
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
