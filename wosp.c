// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <stdlib.h>

#include "input.h"
#include "interpreter.h"
#include "output.h"
#include "search.h"
#include "words.h"

int
main(int argc, char *argv[])
{
    TrieNode *trie = NULL;
    char **filenames = NULL;
    Word **words = NULL;

    /* Search options */
    CaseMode case_mode = CM_INSENSITIVE;
    unsigned int edit_dist = 0;
    ProximityMode proximity_mode = PM_INCLUSIVE;

    /* Output options */
    OutputOptions output_options = init_output_options();

    /* Additional options */
    TokenType default_operator_type = TK_OR_OP;

    size_t n_files = read_data(argc, argv, &trie, &filenames, &words);
    interpret_query(argv[1], trie, case_mode, edit_dist, proximity_mode, default_operator_type, output_options);
    free_data(n_files, trie, filenames, words);

    return EXIT_SUCCESS;
}
