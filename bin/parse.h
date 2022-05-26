#ifndef PARSE_H
#define PARSE_H

#include <string>

#include "sofa/number.h"
#include "sofa/forms.h"
#include "sofa/ineq.h"
#include "sofa/context.h"

// <id> is an integer
//
// <num> :== <id> | <id>/<id>
//
// <length> :== dot(<point_sum>,<vector_sum>) 
//   | <opt_signed_point>.x
//   | <opt_signed_point>.y
//
// <point> :== A(<id>) | C(<id>) | x(<id>) | p(<id>,<id>)
//
// <vector> := u(<id>) | v(<id>)
//
// <T_sum> :== <opt_signed_T>(<signed_T>)*
// <opt_signed_T> :== <signed_T> | <T>
// <signed_T> :== +<T> | -<T>
//
// <ineq> :== <length_sum> >= <num> | <length_sum> <= <num>

class Parser {
  public:
    const &SofaContext ctx;

    Parser() = delete;
    Parser(const SofaContext &ctx);
    Parser(const SofaContext &&) = delete;

    LinearForm parse_expr(const std::string &str);
    LinearForm parse_ineq(const std::string &str);

  private:
    const char *str_;
    
    int id();
    QT num();

    LinearForm length();
    LinearForm length_sum();
    LinearForm opt_signed_length();
    LinearForm signed_length();
    
    LinearFormPoint point();
    LinearFormPoint point_sum();
    LinearFormPoint opt_signed_point();
    LinearFormPoint signed_point();

    Vector vector();
    Vector vector_sum();
    Vector opt_signed_vector();
    Vector signed_vector();
};

#endif // PARSE_H
