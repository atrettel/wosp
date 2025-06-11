/* Copyright (C) 2025 Andrew Trettel */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "interpreter.h"
#include "words.h"

/* Search options */
static const bool case_sensitive = false;

/* Query options */
static const TokenType default_operation_type = TK_OR_OP;
static const char default_operation_string[] = "OR";

/* Output options */
static const LanguageElement print_element = LE_LINE;
static const int print_width = 1;

#endif /* CONFIG_H */
