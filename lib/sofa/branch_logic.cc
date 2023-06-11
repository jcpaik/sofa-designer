#include "branch_logic.h"

// case where point p_i = p(i, i - n) is inside triangular region
void add_corner_inside(
    SofaState &s, int i, int j, Sink &sink);
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

void add_corner(SofaState &s, int i, Sink &sink, bool extend) {
  int n = s.ctx.n();
  { // case when p(i, i - n) under the line y = 0
    SofaState s_below = s.split(s.ctx.is_over(i - n, i, 0));
    if (s_below.is_valid()) {
      std::cout << "WARNING: Below valid" << std::endl;
      sink.push_back(s_below);
    }
  }
  { // case when p(i, i - n) is over the line y = 0
    int n = s.ctx.n();
    int m = int(s.e().size()) - 1;
    for (int j = 0; j <= m; j++) {
      if (!s.is_valid())
        break;
      auto cur = j < m ? s.split(s.ctx.is_left(s.e(j), s.e(j + 1), i)) : s;
      if (!cur.is_valid())
        continue;
      // handle case when p(i, i - n) is under the trapezoids
      if (0 < j && j < m) {
        auto under = cur.split(s.ctx.is_over(i, i - n, s.e(j)));
        if (under.is_valid()) {
          if (extend)
            add_corner_inside(under, i, j, sink);
          else
            sink.push_back(under);
        }
        if (!cur.is_valid())
          continue;
      }
      // now handle case when it's over
      add_corner_outside(cur, i, j, sink, extend);
    }
  }
}

// Adding the corner p(i, i - n) inside the j'th trapezodial
void add_corner_inside(SofaState &s, int i, int j, Sink &sink) {
  int n = s.ctx.n();
  // slope of the upper side of trapezodial
  int k = s.e(j);
  if (k > 0) {
    if (i < k) {
      // extend the left side
      extend_line_left_over(s, i, j, sink);
    } else {
      // the line i is subsumed by niche with edge k
      // so extend only the right side
      extend_line_right_over(s, i - n, j, sink);
    }
  } else {
    if (i - n < k) {
      // extend the left side
      extend_line_left_over(s, i, j, sink);
    } else {
      // the line i is subsumed by 
      extend_line_right_over(s, i - n, j, sink);
    }
  }
}

// Adding the corner p(i, i - n) above the j'th trapezodial
void add_corner_outside(SofaState &s, int i, int j, Sink &sink, bool extend) {
  int n = s.ctx.n();
  int m = int(s.e().size()) - 1;

  // the rays i and i - n hits the niche at some edge
  // ray i at edge 0 <= l <= j, and ray i - n at edge j <= r <= m

  // outer loop for deciding l
  for (int l = j; l >= 0; l--) {
    if (!s.is_valid())
      break;
    // left edge l decided: v(l-1) is over, v(l) ..., v(j-1) are under
    auto sl = l > 0 ? s.split(s.ctx.is_under(s.e(l-1), s.e(l), i)) : s;
    if (!sl.is_valid())
      continue;
    // inner loop for deciding r
    for (int r = j; r <= m; r++) {
      if (!sl.is_valid())
        break;
      // right edge r decided
      auto slr = r < m ? sl.split(s.ctx.is_under(s.e(r), s.e(r+1), i-n)) : sl;
      // update niche edges
      auto e = slr.e();
      if (l == r) {
        e.insert(e.begin() + l + 1, slr.e(l));
        e.insert(e.begin() + l + 1, i - n);
        e.insert(e.begin() + l + 1, i); // e, i, i-n, e
      } else {
        e.erase(e.begin() + l + 1, e.begin() + r);
        e.insert(e.begin() + l + 1, i - n);
        e.insert(e.begin() + l + 1, i);
      }
      slr.update_e(e);

      if (!slr.is_valid())
        continue;
      if (extend)
        extend_line_left_right(slr, i, sink);
      else
        sink.push_back(slr);
    }
  }
}

// extends the inner corner
void extend_line_left_right(
    SofaState &s, int i, Sink &sink) {
  int n = s.ctx.n();

  Sink sls;
  int l = 0;
  while (l < s.e().size() && s.e(l) != i) 
    l++;
  assert(l + 1 < s.e().size() && s.e(l + 1) == i - n);
  extend_line_left_over(s, i, l - 1, sls);

  for (auto &sl : sls) {
    int r = 0;
    while (r < sl.e().size() && sl.e(r) != i - n)
      r++;
    assert(sl.e(r - 1) == i);
    extend_line_right_over(sl, i - n, r + 1, sink);
  }
}

// (pl, pl + 1) over the line `l`. want to divide the case by
// checking whether (pl - 1, pl) is over 'l' or not
void extend_line_left_over(SofaState &s, int l, int pl, Sink &sink) {
  if (pl <= 0) {
    assert(pl == 0);
    if (s.is_valid())
      sink.push_back(s);
  } else { // pl > 0
    assert(s.e(pl) != l);
    if (s.e(pl) < l) { // no possibility for going under
      extend_line_left_over(s, l, pl - 1, sink);
    } else { // s.e(cl) > i, so check possibility for left_under
      SofaState s_under = s.split(s.ctx.is_over(s.e(pl - 1), s.e(pl), l));
      if (s.is_valid())
        extend_line_left_over(s, l, pl - 1, sink);
      if (s_under.is_valid())
        extend_line_left_under(s_under, l, pl - 1, pl, sink);
    }
  }
}

// (pl, pl + 1) under the line `l`. want to divide the case by
// checking whether (pl - 1, pl) is under 'l' or not
// it is known that 'l' has been under since cl: (cl, cl + 1) over, (cl - 1, cl) under
void extend_line_left_under(
    SofaState &s, int l, int pl, int cl, Sink &sink) {
  if (pl <= 0) {
    assert(pl == 0);
    auto e = s.e();
    e.erase(e.begin() + 1, e.begin() + cl);
    e.insert(e.begin() + 1, l);
    s.update_e(e);
    if (s.is_valid())  
      sink.push_back(s);
  } else { // cl > 0
    assert(s.e(pl) != l);
    if (s.e(pl) > l) { // still under
      extend_line_left_under(s, l, pl - 1, cl, sink);
    } else { 
      SofaState s_over = s.split(s.ctx.is_under(s.e(pl - 1), s.e(pl), l));
      if (s.is_valid())
        extend_line_left_under(s, l, pl - 1, cl, sink);
      if (s_over.is_valid()) {
        // pl ... cl
        auto e = s_over.e();
        e.erase(e.begin() + pl + 1, e.begin() + cl);
        e.insert(e.begin() + pl + 1, l);
        s_over.update_e(e);
        if (s_over.is_valid())
          extend_line_left_over(s_over, l, pl - 1, sink);
      }
    }
  }
}

// (pl - 1, pl) over the line `l`. want to divide the case by
// checking whether (pl, pl + 1) is over 'l' or not
void extend_line_right_over(
    SofaState &s, int l, int pl, Sink &sink) {
  int m = int(s.e().size()) - 1;
  if (pl >= m) {
    assert(pl == m);
    if (s.is_valid())
      sink.push_back(s);
  } else {
    assert(s.e(pl) != l);
    if (s.e(pl) > l) {
      extend_line_right_over(s, l, pl + 1, sink);
    } else {
      SofaState s_under = s.split(s.ctx.is_over(s.e(pl), s.e(pl + 1), l));
      if (s.is_valid())
        extend_line_right_over(s, l, pl + 1, sink);
      if (s_under.is_valid())
        extend_line_right_under(s_under, l, pl + 1, pl, sink);
    }
  }
}

// (pl - 1, pl) under the line `l`. want to divide the case by
// checking whether (pl, pl + 1) is under 'l' or not
// it is known that 'l' has been under since cl: (cl - 1, cl) over, (cl, cl + 1) under
void extend_line_right_under(
    SofaState &s, int l, int pl, int cl, Sink &sink) {
  int m = int(s.e().size()) - 1;
  if (pl >= m) {
    assert(pl == m);
    auto e = s.e();
    e.erase(e.begin() + cl + 1, e.begin() + pl);
    e.insert(e.begin() + cl + 1, l);
    s.update_e(e);
    if (s.is_valid()) {
      sink.push_back(s);
    }
  } else { // pl < m
    assert(s.e(pl) != l);
    if (s.e(pl) < l) { // still under
      extend_line_right_under(s, l, pl + 1, cl, sink);
    } else { // consider over
      SofaState s_over = s.split(s.ctx.is_under(s.e(pl), s.e(pl + 1), l));
      if (s.is_valid())
        extend_line_right_under(s, l, pl + 1, cl, sink);
      if (s_over.is_valid()) {
        // cl ... pl
        auto e = s_over.e();
        e.erase(e.begin() + cl + 1, e.begin() + pl);
        e.insert(e.begin() + cl + 1, l);
        s_over.update_e(e);
        if (s_over.is_valid())
          extend_line_right_over(s_over, l, cl + 2, sink);
      }
    }
  }
}
