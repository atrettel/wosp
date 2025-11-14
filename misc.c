// SPDX-License-Identifier: GPL-3.0-or-later
/* Copyright (C) 2025 Andrew Trettel */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

void *
allocmem(size_t len, size_t size)
{
    void *tmp = malloc(len * size);
    if (tmp == NULL)
    {
        fprintf(stderr, "%s: error allocating memory\n", program_name);
        exit(EXIT_FAILURE);
    }
    return tmp;
}

void *
reallocmem(void *data, size_t len)
{
    void *tmp = realloc(data, len);
    if (tmp == NULL)
    {
        fprintf(stderr, "%s: error reallocating memory\n", program_name);
        exit(EXIT_FAILURE);
    }
    return tmp;
}
