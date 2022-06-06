#pragma once

#include <cassert>
#include <vector>
#include <iostream>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"

bool is_negative_semidefinite(const QuadraticForm &q);

bool is_negative_semidefinite(const std::vector< std::vector<QT> > &a);

struct NonnegativeQPSolution {
  CGAL::Quadratic_program_status status;
  std::vector<QT> variables;
  QT value;
};

NonnegativeQPSolution nonnegative_maximize_quadratic_form(
    const QuadraticForm &q, 
    const SofaContext &ctx,
    SofaConstraints cons,
    const std::vector<LinearInequality> &extra_ineqs = {});
