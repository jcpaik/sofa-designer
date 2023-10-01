#pragma once

#include <string>
#include <vector>
#include <sstream>

#include <json/json.h>

#include "expect.h"
#include "number.h"

inline Json::Value to_json(const int &n) {
  return n;
}

inline Json::Value to_json(const NT &n) {
  return n.get_str();
}

inline Json::Value to_json(const QT &q) {
  return q.numerator().get_str() + "/" + q.denominator().get_str();
}

template <typename T>
inline Json::Value to_json(const std::vector<T> &vec) {
  Json::Value arr = Json::arrayValue;
  for (const auto &val : vec)
    arr.append(to_json(val));
  return arr;
}

inline QT stoq(const std::string &str) {
  std::istringstream buf(str);
  QT ret;
  buf >> ret;
  expect(buf.eof());
  return ret;
}

inline QT qt_from_json(const Json::Value val) {
  expect(val.isString());
  return stoq(val.asString());
}

inline std::vector<QT> qts_from_json(Json::Value arr) {
  expect(arr.type() == Json::arrayValue);
  size_t n = arr.size();
  std::vector<QT> ret(n);
  for (int i = 0; i < n; i++) {
    ret[i] = qt_from_json(arr[i]);
  }
  return ret;
}

inline std::vector<int> ints_from_json(Json::Value arr) {
  expect(arr.type() == Json::arrayValue);
  size_t n = arr.size();
  std::vector<int> ret(n);
  for (int i = 0; i < n; i++) {
    ret[i] = arr[i].asInt();
  }
  return ret;
}