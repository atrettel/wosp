/* Copyright (C) 2025 Andrew Trettel */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "interpreter.h"
#include "words.h"

/* Search options */
static const bool case_sensitive = false;
static const char wildcard_character = '?';
static const char extended_wildcard_character = '$';

/* Query options */
static const TokenType default_operation_type = OP_OR;
static const char default_operation_string[] = "OR";

/* Output options */
static const LanguageElement print_element = CLAUSE;
static const int print_width = 1;

#endif /* CONFIG_H */
