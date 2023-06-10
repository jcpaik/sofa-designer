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
// case where point p_i = p(i, i - n) is inside triangular region
void add_corner_inside(
    SofaState &s, int i, int j, Sink &sink, bool extend);
// case where point p_i = p(i, i - n) is outside triangular region
void add_corner_outside(
    SofaState &s, int i, int j, Sink &sink, bool extend);

// extends the inner corner
void extend_line_left_right(
    SofaState &s, int i, Sink &sink);

// (pl, pl + 1) over the line `l`. want to divide the case by
// checking whether (pl - 1, pl) is over 'l' or not
void extend_line_left_over(
    SofaState &s, int l, int pl, Sink &sink);
// (pl, pl + 1) under the line `l`. want to divide the case by
// checking whether (pl - 1, pl) is under 'l' or not
// it is known that 'l' has been under since cl: (cl, cl + 1) over, (cl - 1, cl) under
void extend_line_left_under(
    SofaState &s, int l, int pl, int cl, Sink &sink);
// (pl - 1, pl) over the line `l`. want to divide the case by
// checking whether (pl, pl + 1) is over 'l' or not
void extend_line_right_over(
    SofaState &s, int l, int pl, Sink &sink);
// (pl - 1, pl) under the line `l`. want to divide the case by
// checking whether (pl, pl + 1) is under 'l' or not
// it is known that 'l' has been under since cl: (cl - 1, cl) over, (cl, cl + 1) under
void extend_line_right_under(
    SofaState &s, int l, int pl, int cl, Sink &sink);
