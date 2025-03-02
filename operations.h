/* Copyright (C) 2025 Andrew Trettel */
#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "search.h"

Match *op_or(Match *, Match *);
Match *op_and(Match *, Match *);
Match *op_not(Match *, Match *);
Match *op_xor(Match *, Match *);

Match *op_near(Match *, Match *, int);
Match *op_adj(Match *, Match *, int);
Match *op_with(Match *, Match *, int);

#endif /* OPERATIONS_H */
