/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    options.maximum = UINT_MAX;
    options.type = OT_MATCHES;
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
        if (has_next_word(end_word) == false)
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
        printf("%s\n", filename_document(current));
        current = next_document(current);
        output_count++;
    }
    free_document_list(documents);
}
