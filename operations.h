/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2025 Andrew Trettel */
#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "search.h"

Match *op_or(Match *, Match *);
Match *op_and(Match *, Match *);
Match *op_not(Match *, Match *);
Match *op_xor(Match *, Match *);

Match *op_adj(  Match *, Match *, int, ProximityMode);
Match *op_near( Match *, Match *, int, ProximityMode);
Match *op_among(Match *, Match *, int, ProximityMode);
Match *op_along(Match *, Match *, int, ProximityMode);
Match *op_with( Match *, Match *, int, ProximityMode);
Match *op_same( Match *, Match *, int, ProximityMode);

Match *op_not_adj(  Match *, Match *, int, ProximityMode);
Match *op_not_near( Match *, Match *, int, ProximityMode);
Match *op_not_among(Match *, Match *, int, ProximityMode);
Match *op_not_along(Match *, Match *, int, ProximityMode);
Match *op_not_with( Match *, Match *, int, ProximityMode);
Match *op_not_same( Match *, Match *, int, ProximityMode);

#endif /* OPERATIONS_H */
