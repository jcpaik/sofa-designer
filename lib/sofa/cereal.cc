#include "cereal.h"

#include <cassert>
#include <cstdlib>
#include <vector>

#include "tqdm.h"

#include "number.h"
#include "forms.h"

CerealWriter &operator<<(CerealWriter &out, bool v) {
  out.write((char *)(&v), sizeof(v));
  return out;
}

CerealReader &operator>>(CerealReader &in, bool &v) {
  in.read((char *)(&v), sizeof(v));
  return in;
}

CerealWriter &operator<<(CerealWriter &out, int v) {
  out.write((char *)(&v), sizeof(v));
  return out;
}

CerealReader &operator>>(CerealReader &in, int &v) {
  in.read((char *)(&v), sizeof(v));
  return in;
}

CerealWriter &operator<<(CerealWriter &out, size_t v) {
  out.write((char *)(&v), sizeof(v));
  return out;
}

CerealReader &operator>>(CerealReader &in, size_t &v) {
  in.read((char *)(&v), sizeof(v));
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const NT &v) {
  out << (v < 0);

  size_t c;
  void *raw = mpz_export( nullptr, &c, 1, 1, 0, 0, v.get_mpz_t() );
  out << c;
  out.write((char*)raw, c);
  std::free(raw);
  return out;
}

CerealReader &operator>>(CerealReader &in, NT &v) {
  bool neg;
  in >> neg;

  size_t s;
  in >> s;
  std::vector<uint8_t> buf(s);
  in.read((char*)&buf[0], s);
  mpz_import(v.get_mpz_t(), s, 1, 1, 0, 0, &buf[0]);

  if (neg) {
    v = -v;
  }
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const QT &v) {
  out << v.numerator() << v.denominator();
  return out;
}

CerealReader &operator>>(CerealReader &in, QT &v) {
  in >> v.num >> v.den;
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const LinearForm &v) {
  out << v.d();
  out << v.w0();
  for (int i = 0; i < v.d(); i++)
    out << v.w1(i);
  return out;
}

CerealReader &operator>>(CerealReader &in, LinearForm &v) {
  int d;
  in >> d;
  v.d_ = d;
  in >> v.w0_;
  v.w1_.resize(d);
  for (int i = 0; i < d; i++)
    in >> v.w1_[i];
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const QuadraticForm &v) {
  out << v.d();
  out << v.w0();
  for (int i = 0; i < v.d(); i++)
    out << v.w1(i);
  for (int i = 0; i < v.d(); i++)
    for (int j = 0; j <= i; j++)
      out << v.w2(i, j);
  return out;
}

CerealReader &operator>>(CerealReader &in, QuadraticForm &v) {
  int d;
  in >> d;
  v.d_ = d;
  in >> v.w0_;
  v.w1_.resize(d);
  for (int i = 0; i < d; i++)
    in >> v.w1_[i];
  v.w2_.resize(d);
  for (int i = 0; i < d; i++) {
    v.w2_[i].resize(i + 1);
    for (int j = 0; j <= i; j++) {
      in >> v.w2_[i][j];
    }
  }
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const LinearFormPoint &v) {
  out << v.x << v.y;
  return out;
}

CerealReader &operator>>(CerealReader &in, LinearFormPoint &v) {
  in >> v.x >> v.y;
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const Vector &v) {
  out << v.x << v.y;
  return out;
}

CerealReader &operator>>(CerealReader &in, Vector &v) {
  in >> v.x >> v.y;
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const SofaContext &v) {
  int n = v.n_;
  std::vector<Vector> u(v.u_.begin() + 1, v.u_.begin() + n);
  out << u;
  return out;
}

CerealReader &operator>>(CerealReader &in, SofaContext &v) {
  std::vector<Vector> u;
  in >> u;
  v.initialize(u);
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const SofaState &v) {
  out << v.is_valid_;
  out << v.e_;
  out << v.conds_;
  out << v.area_;
  out << v.vars_;
  return out;
}

CerealReader &operator>>(CerealReader &in, SofaState &v) {
  in >> v.is_valid_;
  in >> v.e_;
  in >> v.conds_;
  in >> v.area_;
  in >> v.vars_;
  return in;
}

CerealWriter &operator<<(CerealWriter &out, const SofaBranchTree &v) {
  out << v.n;
  out << v.valid_states_;
  return out;
}

CerealReader &operator>>(CerealReader &in, SofaBranchTree &v) {
  in >> v.n;
  v.valid_states_.clear();
  size_t sz;
  in >> sz;
  tqdm bar;
  for (size_t i = 0; i < sz; i++) {
    bar.progress(i, sz);
    v.valid_states_.push_back(SofaState(v.ctx, in));
  }
  bar.finish();
  return in;
}
