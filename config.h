/* Copyright (C) 2025 Andrew Trettel */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <limits.h>

#include "interpreter.h"
#include "output.h"
#include "words.h"

/* Output options */
static const LanguageElement print_element = LE_LINE;
static const int print_before = 1;
static const int print_after = 1;
static const bool print_filename = true;
static const bool print_line_number = true;
static const unsigned int print_maximum = UINT_MAX;
static const OutputType print_type = OT_MATCHES;

/* Additional options */
static const char program_name[] = "wosp";
static const bool output_syntax_tree = false;
static const TokenType default_operator_type = TK_OR_OP;

#endif /* CONFIG_H */
