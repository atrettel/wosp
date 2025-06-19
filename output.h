/* Copyright (C) 2025 Andrew Trettel */
#ifndef OUTPUT_H
#define OUTPUT_H

#include "search.h"

typedef enum OutputType
{
    OT_DOCUMENTS,
    OT_MATCHES
} OutputType;

typedef struct OutputOptions
{
    LanguageElement element;
    int before;
    int after;
    bool filename;
    bool line_number;
    unsigned int maximum;
    OutputType type;
} OutputOptions;

OutputOptions init_output_options(void);
LanguageElement element_output_options(OutputOptions);
int before_output_options(OutputOptions);
int after_output_options(OutputOptions);
bool filename_output_options(OutputOptions);
bool line_number_output_options(OutputOptions);
unsigned int maximum_output_options(OutputOptions);
OutputType type_output_options(OutputOptions);

void print_matches(Match *, OutputOptions);
void print_documents_in_matches(Match *, OutputOptions);

#endif /* OUTPUT_H */
