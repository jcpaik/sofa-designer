#pragma once

#include <vector>
#include <iostream>

#include <boost/optional.hpp>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"

bool is_negative_semidefinite(const QuadraticForm &q);

bool is_negative_semidefinite(const std::vector< std::vector<QT> > &a);

struct SofaAreaQPInvalidProof {
  bool flag;
};

struct SofaAreaQPSolution {
  std::vector<QT> variables;
  QT value;
  CGAL::Quadratic_program_status status;
  // dummy entry - to check if boost::optional works
  boost::optional<SofaAreaQPInvalidProof> invalid_proof;
};

SofaAreaQPSolution sofa_area_qp(
    const QuadraticForm &area,
    const SofaContext &ctx,
    SofaConstraints cons,
    const std::vector<LinearInequality> &extra_ineqs = {});
