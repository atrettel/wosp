/* Copyright (C) 2025 Andrew Trettel */
#ifndef OUTPUT_H
#define OUTPUT_H

#include "search.h"

typedef enum OutputType
{
    OT_DOCUMENTS,
    OT_EXCERPTS,
    OT_MATCHES
} OutputType;

typedef struct OutputOptions
{
    LanguageElement element;
    int before;
    int after;
    bool filename;
    bool line_number;
    bool page_number;
    bool count_matches;
    unsigned int maximum;
    OutputType type;
} OutputOptions;

OutputOptions init_output_options(void);
LanguageElement element_output_options(OutputOptions);
int before_output_options(OutputOptions);
int after_output_options(OutputOptions);
bool filename_output_options(OutputOptions);
bool line_number_output_options(OutputOptions);
bool page_number_output_options(OutputOptions);
bool count_matches_output_options(OutputOptions);
unsigned int maximum_output_options(OutputOptions);
OutputType type_output_options(OutputOptions);

typedef enum ExcerptStatus
{
    ES_EXCLUDE,
    ES_INCLUDE,
    ES_MATCH,
} ExcerptStatus;

void print_matches(Match *, OutputOptions);
void print_documents_in_matches(Match *, OutputOptions);
void print_excerpts(Match *, OutputOptions);

#endif /* OUTPUT_H */
