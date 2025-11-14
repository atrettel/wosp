/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2025 Andrew Trettel */
#ifndef MISC_H
#define MISC_H

#include <stdlib.h>

static const char program_name[]    = "wosp";
static const char program_version[] = "0.0.0";

void *allocmem(size_t, size_t);
void *reallocmem(void *, size_t);

#endif /* MISC_H */
