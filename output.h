/* Copyright (C) 2025 Andrew Trettel */
#ifndef OUTPUT_H
#define OUTPUT_H

#include "search.h"

void print_matches(Match *);
void print_documents_in_matches(Match *);

typedef enum OutputType
{
    OT_DOCUMENTS,
    OT_MATCHES
} OutputType;

#endif /* OUTPUT_H */
