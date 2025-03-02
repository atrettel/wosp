/* Copyright (C) 2025 Andrew Trettel */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "words.h"

/* Search options */
static const bool case_sensitive = false;
static const char wildcard_character = '?';

/* Output options */
static const LanguageElement print_element = CLAUSE;
static const int print_width = 1;

#endif /* CONFIG_H */
