/* Copyright (C) 2025 Andrew Trettel */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "interpreter.h"
#include "words.h"

/* Search options */
static const bool case_sensitive = false;

/* Query options */
static const TokenType default_operator_type = TK_OR_OP;

/* Output options */
static const LanguageElement print_element = LE_LINE;
static const int print_before = 1;
static const int print_after = 1;
static const bool print_filename = true;
static const bool print_line_number = true;

/* Debug options */
static const bool debug_syntax_tree = true;

#endif /* CONFIG_H */
