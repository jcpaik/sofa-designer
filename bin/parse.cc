#include "parse.h"

#include <exception>

Parser::Parser(const SofaContext &ctx) : ctx(ctx) {}

/*
LinearForm Parser::parse_expr(const std::string &str) {
  p_ = str.c_str();
  return length_sum_();
}

LinearInequality Parser::parse_ineq(const std::string &str) {
  p_ = str.c_str();
  return ineq_();
}
*/

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
    if ('0' > *p_ || *p_ > '9') {
      break;
    }
    id = 10 * id + (int(*p_) - int('0'));
  }

  return id;
}
