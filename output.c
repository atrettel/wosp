/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "search.h"
#include "words.h"

void
print_matches(Match *match)
{
    Match *current_match = match;
    unsigned int output_count = 0;
    while (is_match(current_match) == true && output_count < print_maximum)
    {
        int start_n = -print_before;
        int end_n   = +print_after;
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
        if (print_filename == true)
        {
            printf("%s:", filename_word(start_word));
        }
        if (print_line_number == true)
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
print_documents_in_matches(Match *match)
{
    unsigned int output_count = 0;
    DocumentNode *documents = document_list_match_list(match);
    DocumentNode *current = documents;
    while (is_document(current) == true && output_count < print_maximum)
    {
        printf("%s\n", filename_document(current));
        current = next_document(current);
        output_count++;
    }
    free_document_list(documents);
}
