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
//   | <point>.x
//   | <point>.y
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
    const SofaContext &ctx;

    Parser() = delete;
    Parser(const SofaContext &ctx);
    Parser(const SofaContext &&) = delete;

    LinearForm parse_expr(const std::string &str);
    LinearInequality parse_ineq(const std::string &str);

  private:
    const char *p_;

    void expect_(const char * const str);
    
    int id_();
    QT num_();

    LinearFormPoint point_();
    LinearFormPoint signed_point_();
    LinearFormPoint opt_signed_point_();
    LinearFormPoint point_sum_();

    Vector vector_();
    Vector signed_vector_();
    Vector opt_signed_vector_();
    Vector vector_sum_();

    LinearForm length_();
    LinearForm signed_length_();
    LinearForm opt_signed_length_();
    LinearForm length_sum_();
    
    LinearInequality ineq_();
};

#endif // PARSE_H
