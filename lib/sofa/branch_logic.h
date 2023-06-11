#pragma once

#include <vector>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"
#include "qp.h"
#include "state.h"


typedef std::vector<SofaState> Sink;

// function call = case division
// push to sink = termination
void add_corner(
    SofaState &s, int i, Sink &sink, bool extend);