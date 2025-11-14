// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "output.h"
#include "search.h"
#include "words.h"

OutputOptions init_output_options(void)
{
    OutputOptions options;
    options.element = LE_LINE;
    options.before = 1;
    options.after = 1;
    options.filename = true;
    options.line_number = true;
    options.page_number = false;
    options.count_matches = false;
    options.maximum = UINT_MAX;
    options.type = OT_EXCERPTS;
    return options;
}

LanguageElement element_output_options(OutputOptions options)
{
    return options.element;
}

int before_output_options(OutputOptions options)
{
    return options.before;
}

int after_output_options(OutputOptions options)
{
    return options.after;
}

bool filename_output_options(OutputOptions options)
{
    return options.filename;
}

bool line_number_output_options(OutputOptions options)
{
    return options.line_number;
}

bool page_number_output_options(OutputOptions options)
{
    return options.page_number;
}

bool count_matches_output_options(OutputOptions options)
{
    return options.count_matches;
}

unsigned int maximum_output_options(OutputOptions options)
{
    return options.maximum;
}

OutputType type_output_options(OutputOptions options)
{
    return options.type;
}

void
print_matches(Match *match, OutputOptions options)
{
    Match *current_match = match;
    unsigned int output_count = 0;
    while (is_match(current_match) == true && output_count < maximum_output_options(options))
    {
        LanguageElement print_element = element_output_options(options);
        int start_n = -before_output_options(options);
        int end_n   = +after_output_options(options);
        if (print_element == LE_WORD)
        {
            start_n += 1;
        }
        Word *start_word = advance_word(start_word_match(current_match), print_element, start_n);
        Word   *end_word = advance_word(  end_word_match(current_match), print_element,   end_n);
        if (field_has_next_word(end_word) == false)
        {
            end_word = NULL;
        }
        if (filename_output_options(options) == true)
        {
            printf("%s:", filename_word(start_word));
        }
        if (page_number_output_options(options) == true)
        {
            printf("%lu:", page_word(start_word));
        }
        if (line_number_output_options(options) == true)
        {
            printf("%lu:", line_word(start_word));
        }
        Word *current_word = start_word;
        while (current_word != end_word)
        {
            if (current_word != start_word)
            {
                printf(" ");
            }
            printf("%s", original_word(current_word));
            current_word = next_word(current_word);
        }
        printf("\n");
        current_match = next_match(current_match);
        output_count++;
    }
}

void
print_documents_in_matches(Match *match, OutputOptions options)
{
    unsigned int output_count = 0;
    DocumentNode *documents = document_list_match_list(match);
    DocumentNode *current = documents;
    while (is_document(current) == true && output_count < maximum_output_options(options))
    {
        printf("%s", filename_document(current));
        if (count_matches_output_options(options) == true)
        {
            unsigned long count = 0;
            Match *current_match = match;
            while (is_match(current_match) == true)
            {
                if (document_match(current_match) == document_document(current))
                {
                    count++;
                }
                current_match = next_match(current_match);
            }
            printf(":%lu", count);
        }
        printf("\n");
        current = next_document(current);
        output_count++;
    }
    free_document_list(documents);
}

void
print_excerpts(Match *match, OutputOptions options)
{
    unsigned int output_count = 0;
    DocumentNode *documents = document_list_match_list(match);
    DocumentNode *current_document = documents;
    while (is_document(current_document) == true && output_count < maximum_output_options(options))
    {
        Word *words = list_first_word(document_document(current_document));
        size_t n_words = (size_t) position_word(list_last_word(words));

        ExcerptStatus *word_print = (ExcerptStatus *) allocmem(n_words, sizeof(ExcerptStatus));
        for (size_t i = 0; i < n_words; i++)
        {
            word_print[i] = ES_EXCLUDE;
        }
        Match *current_match = match;
        while (is_match(current_match) == true)
        {
            if (document_match(current_match) == document_document(current_document))
            {
                size_t n_match = number_of_words_in_match(current_match);
                for (size_t i = 0; i < n_match; i++)
                {
                    Word *current_word = word_match(current_match, i);
                    word_print[(size_t) position_word(current_word) - 1] = ES_MATCH;
                }
            }
            current_match = next_match(current_match);
        }

        Word *current_word = words;
        for (size_t i = 0; i < n_words; i++)
        {
            if (word_print[i] == ES_MATCH)
            {
                Word *start_word = advance_word(current_word, element_output_options(options), -before_output_options(options));
                Word *end_word   = advance_word(current_word, element_output_options(options),  +after_output_options(options));
                size_t i_start = (size_t) position_word(start_word) - 1;
                size_t i_end   = (size_t) position_word(end_word)   - 1;
                for (size_t j = i_start; j < i_end; j++)
                {
                    if (word_print[j] == ES_EXCLUDE)
                    {
                        word_print[j] = ES_INCLUDE;
                    }
                }
            }
            current_word = next_word(current_word);
        }

        current_word = words;
        Word *start_word = words;
        bool prev_print = false;
        unsigned int excerpt_count = 0;
        while (is_end_field(current_word) == false)
        {
            size_t i = (size_t) position_word(current_word) - 1;
            if (word_print[i] == ES_EXCLUDE)
            {
                if (prev_print == true)
                {
                    if (count_matches_output_options(options) == true)
                    {
                        excerpt_count++;
                    }
                    else
                    {
                        printf("\n");
                    }
                    output_count++;
                }
                prev_print = false;
            }
            else
            {
                if (count_matches_output_options(options) == true)
                {
                    if (prev_print == false)
                    {
                        prev_print = true;
                    }
                }
                else
                {
                    if (prev_print == false)
                    {
                        if (filename_output_options(options) == true)
                        {
                            printf("%s:", filename_word(current_word));
                        }
                        if (page_number_output_options(options) == true)
                        {
                            printf("%lu:", page_word(current_word));
                        }
                        if (line_number_output_options(options) == true)
                        {
                            printf("%lu:", line_word(current_word));
                        }
                        start_word = current_word;
                        prev_print = true;
                    }
                    if (current_word != start_word)
                    {
                        printf(" ");
                    }
                    printf("%s", original_word(current_word));
                }
            }
            current_word = next_word(current_word);
        }

        if (count_matches_output_options(options) == true)
        {
            printf("%s:%u\n", filename_document(current_document), excerpt_count);
        }
        free(word_print);
        current_document = next_document(current_document);
    }
    free_document_list(documents);
}
