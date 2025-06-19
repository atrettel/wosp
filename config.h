/* Copyright (C) 2025 Andrew Trettel */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <limits.h>

#include "interpreter.h"
#include "output.h"
#include "words.h"

/* Additional options */
static const char program_name[] = "wosp";
static const bool output_syntax_tree = false;
static const TokenType default_operator_type = TK_OR_OP;

#endif /* CONFIG_H */
