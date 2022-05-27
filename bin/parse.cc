#include "parse.h"

Parser::Parser(const SofaContext &ctx) : ctx(ctx) {}

LinearForm Parser::parse_expr(const std::string &str) {
  c_ = str.c_str();
  return length_sum_();
}

LinearForm Parser::parse_ineq(const std::string &str) {
  c_ = str.c_str();
  return ineq_();
}
