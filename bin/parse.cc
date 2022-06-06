#include "parse.h"

#include <cstring>
#include <string>
#include <exception>

Parser::Parser(const SofaContext &ctx) : ctx(ctx) {}

LinearForm Parser::parse_expr(const std::string &str) {
  p_ = str.c_str();
  return length_sum_();
}

/*
LinearInequality Parser::parse_ineq(const std::string &str) {
  p_ = str.c_str();
  return ineq_();
}
*/

void Parser::expect_(const char * const str) {
  size_t n = strlen(str);
  if (strncmp(p_, str, n) != 0) {
    throw std::runtime_error(std::string(str) + " expected");
  }
  p_ += n;
}

int Parser::id_() {
  // Parse zero
  if (*p_ == '0') {
    p_++;
    return 0;
  }

  // Parse optional negative sign
  bool negate = (*p_ == '-');
  if (negate) {
    p_++;
  }

  if ('1' > *p_ || *p_ > '9') {
    throw std::range_error("Leading zero not found");
  }

  int id = int(*p_) - int('0');
  assert(1 <= id && id <= 9);
  const int MAX_DIGITS = 8;
  for (int i = 0; i < MAX_DIGITS; i++) {
    // Pass if non-digit
    p_++;
    if ('0' > *p_ || *p_ > '9') {
      break;
    }
    id = 10 * id + (int(*p_) - int('0'));
  }

  return id;
}

QT Parser::num_() {
  int numer = id_();
  if (*p_ == '/') {
    p_++;
    int denom = id_();
    return QT{numer, denom};
  } else {
    return QT{numer};
  }
}

LinearFormPoint Parser::point_() {
  if (*p_ == 'A') {
    p_++;
    expect_("(");
    int id = id_();
    expect_(")");
    return ctx.A(id);
  } else if (*p_ == 'C') {
    p_++;
    expect_("(");
    int id = id_();
    expect_(")");
    return ctx.C(id);
  } else if (*p_ == 'x') {
    p_++;
    expect_("(");
    int id = id_();
    expect_(")");
    return ctx.x(id);
  } else if (*p_ == 'p') {
    p_++;
    expect_("(");
    int id0 = id_();
    expect_(",");
    int id1 = id_();
    expect_(")");
    return ctx.p(id0, id1);
  } else {
    throw std::runtime_error("A,C,x,p expected for point");
  }
}

LinearFormPoint Parser::signed_point_() {
  if (*p_ == '+') {
    p_++;
    return point_();
  } else {
    expect_("-");
    return -point_();
  }
}

LinearFormPoint Parser::opt_signed_point_() {
  if (*p_ == '+' || *p_ == '-') {
    return signed_point_();
  } else {
    return point_();
  }
}

LinearFormPoint Parser::point_sum_() {
  LinearFormPoint pt = opt_signed_point_();
  while (*p_ == '+' || *p_ == '-')
    pt += signed_point_();
  return pt;
}

Vector Parser::vector_() {
  if (*p_ == 'u') {
    expect_("(");
    int id = id_();
    expect_(")");
    return ctx.u(id);
  } else if (*p_ == 'v') {
    expect_("(");
    int id = id_();
    expect_(")");
    return ctx.v(id);
  } else {
    throw std::runtime_error("u or v expected");
  }
}

Vector Parser::signed_vector_() {
  if (*p_ == '+') {
    p_++;
    return vector_();
  } else {
    expect_("-");
    return -vector_();
  }
}

Vector Parser::opt_signed_vector_() {
  if (*p_ == '+' || *p_ == '-') {
    return signed_vector_();
  } else {
    return vector_();
  }
}

Vector Parser::vector_sum_() {
  Vector vec = opt_signed_vector_();
  while (*p_ == '+' || *p_ == '-')
    vec += signed_vector_();
  return vec;
}

LinearForm Parser::length_() {
  if (*p_ == 'd') {
    expect_("dot(");
    LinearFormPoint pt = point_sum_();
    expect_(",");
    Vector vec = vector_sum_();
    expect_(")");
    return dot(pt, vec);
  } else {
    LinearFormPoint pt = point_();
    expect_(".");
    if (*p_ == 'x') {
      p_++;
      return pt.x;
    } else if (*p_ == 'y') {
      p_++;
      return pt.y;
    } else {
      throw std::runtime_error("x or y expected");
    }
  }
}

LinearForm Parser::signed_length_() {
  if (*p_ == '+') {
    p_++;
    return length_();
  } else {
    expect_("-");
    return -length_();
  }
}

LinearForm Parser::opt_signed_length_() {
  if (*p_ == '+' || *p_ == '-') {
    return signed_length_();
  } else {
    return length_();
  }
}

LinearForm Parser::length_sum_() {
  LinearForm len = opt_signed_length_();
  while (*p_ == '+' || *p_ == '-')
    len += signed_length_();
  return len;
}
