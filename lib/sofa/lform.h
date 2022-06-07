#pragma once

#include <vector>

#include "number.h"

class QuadraticForm;
class CerealReader;

class LinearForm {
  public:
    // Constructors
    LinearForm();
    explicit LinearForm(int d);
    LinearForm(const QT &w0, const std::vector<QT> &w1);

    // Factory methods
    static LinearForm constant(int d, const QT &c);
    static LinearForm variable(int d, int i);

    // Accessors
    int d() const;

    QT &w0();
    const QT &w0() const;

    std::vector<QT> &w1();
    const std::vector<QT> &w1() const;
    QT &w1(int i);
    const QT &w1(int i) const;

    // Arithmetic
    LinearForm &operator+=(const LinearForm &other);
    LinearForm &operator-=(const LinearForm &other);
    LinearForm &operator*=(const QT &c);
    LinearForm &operator/=(const QT &c);

    LinearForm operator+(const LinearForm &other) const;
    LinearForm operator-(const LinearForm &other) const;
    LinearForm operator*(const QT &c) const;
    LinearForm operator/(const QT &c) const;

    QuadraticForm operator*(const LinearForm &other) const;

    LinearForm operator-() const;

    bool operator==(const LinearForm &other) const;
    bool operator!=(const LinearForm &other) const;

    LinearForm &normalize();

    // Substitution
    QT operator()(std::vector<QT> values) const;

    // Serialization
    // Only this function can change the dimension d()
    friend CerealReader &operator>>(CerealReader &in, LinearForm &v);

  private:
    int d_;
    QT w0_;
    std::vector<QT> w1_;
};

// Arithmetic
LinearForm operator*(const QT &c, const LinearForm &f);
