#include "branch_logic.h"


void add_corner(SofaState &s, int i, Sink &sink,
    bool extend_in, bool extend_out) {
  int n = s.ctx.n();
  { // case when p(i, i - n) under the line y = 0
    SofaState s_below = s.split(s.ctx.is_over(i - n, i, 0));
    if (s_below.is_valid()) {
      std::cout << "WARNING: Below valid" << std::endl;
      sink.push_back(s_below);
    }
  }
  { // case when p(i, i - n) inside the union of triangles
    SofaState s_inside = s;
    add_corner_inside(s_inside, i, sink, extend_in);
  }
  { // case when p(i, i - n) outside the union of triangles
    add_corner_outside(s, i, sink, extend_out);
  }
}

void add_corner_inside(SofaState &s, int i, Sink &sink, bool extend) {
  int n = s.ctx.n();
  // divide everything into trapezodial regions
  // number of trapezodials
  int m = int(s.e().size()) - 1;
  // s.impose(s.p(0).x <= cp.x);
  s.impose(s.ctx.is_left(s.e(0), s.e(1), i));
  // s.impose(s.p(m - 1).x >= cp.x);
  s.impose(s.ctx.is_right(s.e(m - 1), s.e(m), i));

  for (int j = 1; j <= m - 1; j++) {
    // case when the point is in j'th trapezodial
    if (!s.is_valid())
      break;
    auto cur = s.split(s.ctx.is_left(s.e(j), s.e(j + 1), i));
    cur.impose(s.ctx.is_under(i, i - n, s.e(j)));
    if (!cur.is_valid())
      continue;
    if (extend) {
      // point cp under the j'th trapezodial
      int k = s.e(j);
      if (k > 0) {
        if (i < k) {
          // extend the left side
          extend_line_left_over(cur, i, j, sink);
        } else {
          // i > k
          // extend the right side
          extend_line_right_over(cur, i - n, j, sink);
        }
      } else {
        // k < 0
        if (i - n < k) {
          // extend the left side
          extend_line_left_over(cur, i, j, sink);
        } else {
          // i - n > k
          // extend the right side
          extend_line_right_over(cur, i - n, j, sink);
        }
      }
    } else {
      sink.push_back(cur);
    }
  }
}

void add_corner_outside(SofaState &s, int i, Sink &sink, bool extend) {
  int n = s.ctx.n();
  int m = int(s.e().size()) - 1;

  // case when rays i, i - n emerging from p(i, i - n) hits s.e(j)
  for (int j = 0; j <= m; j++) {
    if (i - n < s.e(j) && s.e(j) < i) {
      SofaConstraints rels;
      rels.push_back(s.ctx.is_over(i - n, i, s.e(j)));
      if (j > 0)
        rels.push_back(s.ctx.is_over(s.e(j-1), s.e(j), i));
      if (j < m)
        rels.push_back(s.ctx.is_over(s.e(j), s.e(j+1), i - n));
      SofaState sn = s;
      sn.impose(rels);
      auto e = sn.e();
      e.insert(e.begin() + j + 1, sn.e(j));
      e.insert(e.begin() + j + 1, i - n);
      e.insert(e.begin() + j + 1, i); // e, i, i-n, e
      sn.update_e(e);
      if (sn.is_valid()) {
        if (extend)
          extend_line_left_right(sn, i, sink); 
        else
          sink.push_back(sn);
      }
    }
  }

  for (int l = 0; l <= m - 1; l++) {
    if (s.e(l) < i) {
      auto sl = s;
      SofaConstraints rels;
      rels.push_back(s.ctx.is_over(i - n, i, s.e(l)));
      if (l > 0)
        rels.push_back(s.ctx.is_over(s.e(l-1), s.e(l), i));
      rels.push_back(s.ctx.is_under(s.e(l), s.e(l+1), i));
      rels.push_back(s.ctx.is_under(s.e(l), s.e(l+1), i - n));
      sl.impose(rels);

      for (int r = l + 1; r <= m; r++) {
        if (!sl.is_valid())
          break;
        SofaState slr = (r < m) ? sl.split(s.ctx.is_under(s.e(r), s.e(r + 1), i - n)) : sl;
        if (i - n >= s.e(r))
          continue;
        SofaConstraints rels;
        rels.push_back(s.ctx.is_over(i - n, i, s.e(r)));
        for (int j = l + 1; j < r; j++)
          rels.push_back(s.ctx.is_under(s.e(j), s.e(j+1), i));
        slr.impose(rels);
        if (!slr.is_valid())
          continue;
        auto e = slr.e();
        e.erase(e.begin() + l + 1, e.begin() + r);
        e.insert(e.begin() + l + 1, i - n);
        e.insert(e.begin() + l + 1, i);
        slr.update_e(e);
        if (slr.is_valid()) {
          if (extend)
            extend_line_left_right(slr, i, sink);
          else
            sink.push_back(slr);
        }
      }
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
