#pragma once

#include <fstream>
#include <iostream>

#include "number.h"
#include "forms.h"
#include "geom.h"
#include "context.h"
#include "state.h"
#include "branch_tree.h"

// Potential pitfall: 
// operator<< and operator>> might get mixed with
// original implementation of std::ifstream and std::ofstream
// ex) char c; creader >> c; will still behave as std::ifstream
class CerealReader : public std::ifstream {
  public:
    CerealReader() = delete;
    CerealReader(const char *fname) 
        : std::ifstream(fname, std::ios::in | std::ios::binary) {
    }
};

class CerealWriter : public std::ofstream {
  public:
    CerealWriter() = delete;
    CerealWriter(const char *fname)
        : std::ofstream(fname, std::ios::out | 
                               std::ios::binary | 
                               std::ios::trunc) {
    }
};

template <typename T>
void save(const char *filename, const T &obj) {
  CerealWriter writer(filename);
  writer << obj;
  writer.close();
}

template <typename T>
void load(const char *filename, T &obj) {
  CerealReader reader(filename);
  reader >> obj;
  reader.close();
}

CerealWriter &operator<<(CerealWriter &out, bool v);
CerealReader &operator>>(CerealReader &in, bool &v);
CerealWriter &operator<<(CerealWriter &out, int v);
CerealReader &operator>>(CerealReader &in, int &v);
CerealWriter &operator<<(CerealWriter &out, size_t v);
CerealReader &operator>>(CerealReader &in, size_t &v);
CerealWriter &operator<<(CerealWriter &out, const NT &v);
CerealReader &operator>>(CerealReader &in, NT &v);
CerealWriter &operator<<(CerealWriter &out, const QT &v);
CerealReader &operator>>(CerealReader &in, QT &v);
CerealWriter &operator<<(CerealWriter &out, const LinearForm &v);
CerealReader &operator>>(CerealReader &in, LinearForm &v);
CerealWriter &operator<<(CerealWriter &out, const QuadraticForm &v);
CerealReader &operator>>(CerealReader &in, QuadraticForm &v);
CerealWriter &operator<<(CerealWriter &out, const Vector &v);
CerealReader &operator>>(CerealReader &in, Vector &v);
CerealWriter &operator<<(CerealWriter &out, const SofaContext &v);
CerealReader &operator>>(CerealReader &in, SofaContext &v);
CerealWriter &operator<<(CerealWriter &out, const SofaState &v);
CerealReader &operator>>(CerealReader &in, SofaState &v);
CerealWriter &operator<<(CerealWriter &out, const SofaBranchTree &v);
CerealReader &operator>>(CerealReader &in, SofaBranchTree &v);

template <typename T>
CerealWriter &operator<<(CerealWriter &out, const std::vector<T> &vec) {
  out << size_t(vec.size());
  for (const auto &v : vec)
    out << v;
  return out;
}

template <typename T>
CerealReader &operator>>(CerealReader &in, std::vector<T> &vec) {
  vec.clear();
  size_t n;
  in >> n;
  for (size_t i = 0; i < n; i++) {
    T t;
    in >> t;
    vec.push_back(t);
  }
  return in;
}
