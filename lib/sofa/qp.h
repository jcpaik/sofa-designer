#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <utility>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"

bool is_negative_semidefinite(const QuadraticForm &q);

bool is_negative_semidefinite(const std::vector< std::vector<QT> > &a);

struct SofaAreaOptimalityProof {
  QT max_area;
  std::vector<QT> maximizer;
  std::vector< std::vector<QT> > max_transform;
};

struct SofaAreaInvalidityProof {
  std::map<SofaConstraintProbe, QT> weight;
  std::map<int, QT> weight_extra;
};

struct SofaAreaResult {
  std::variant<SofaAreaOptimalityProof, SofaAreaInvalidityProof> result;

  bool is_optimal() const {
    return std::holds_alternative<SofaAreaOptimalityProof>(result);
  }

  explicit operator bool() const {
    return is_optimal() && 
      std::get<SofaAreaOptimalityProof>(result).max_area > QT(22195, 10000);
  }

  const SofaAreaOptimalityProof& optimality_proof() const {
    return std::get<SofaAreaOptimalityProof>(result);
  }

  const SofaAreaInvalidityProof& invalidity_proof() const {
    return std::get<SofaAreaInvalidityProof>(result);
  }
};

SofaAreaResult sofa_area_qp(
    const QuadraticForm &area,
    const SofaContext &ctx,
    SofaConstraints cons,
    const std::vector<LinearInequality> &extra_ineqs = {});
