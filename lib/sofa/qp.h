#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <utility>
#include <optional>
#include <variant>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <json/json.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"

struct CholeskyLDL {
  QTMatrix l;
  std::vector<QT> d;
};

std::optional<CholeskyLDL> is_negative_semidefinite(const QTMatrix &mat);

// Area formula = max_area - lambdas . nonnegative_values - (ldl^T) (vector - maximizer)
struct SofaAreaOptimalityProof {
  QT max_area;
  std::vector<QT> maximizer;
  QTMatrix l;
  std::vector<QT> d;
  std::map<SofaConstraintProbe, QT> lambdas;
  std::map<int, QT> lambdas_extra;

  Json::Value json() const;
};

struct SofaAreaInvalidityProof {
  std::map<SofaConstraintProbe, QT> lambdas;
  std::map<int, QT> lambdas_extra;

  Json::Value json() const;
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
