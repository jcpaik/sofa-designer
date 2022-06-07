#pragma once

#include <vector>

#include "number.h"

class LinearForm;
class CerealReader;

class QuadraticForm {
  public:
    // Constructors
    QuadraticForm();
    explicit QuadraticForm(int d);
    QuadraticForm(const LinearForm &l);
    QuadraticForm(const QT &w0, const std::vector<QT> &w1, 
                  const std::vector< std::vector<QT> > &w2); 

    // Accessors
    int d() const;

    QT &w0();
    const QT &w0() const;

    std::vector<QT> &w1();
    const std::vector<QT> &w1() const;
    QT &w1(int i);
    const QT &w1(int i) const;

    std::vector< std::vector<QT> > &w2();
    const std::vector< std::vector<QT> > &w2() const;
    // when i < j, w2(i, j) is an alias of w2(j, i)
    QT &w2(int i, int j);
    const QT &w2(int i, int j) const;

    // Arithmetic
    QuadraticForm &operator+=(const QuadraticForm &other);
    QuadraticForm &operator-=(const QuadraticForm &other);
    QuadraticForm &operator*=(const QT &c);
    QuadraticForm &operator/=(const QT &c);

    QuadraticForm operator+(const QuadraticForm &other) const;
    QuadraticForm operator-(const QuadraticForm &other) const;
    QuadraticForm operator*(const QT &c) const;
    QuadraticForm operator/(const QT &c) const;

    QuadraticForm operator-() const;

    bool operator==(const QuadraticForm &other) const;
    bool operator!=(const QuadraticForm &other) const;

    QuadraticForm &normalize();

    // Substitution
    QT operator()(std::vector<QT> values) const;

    // Serialization
    // Only this function can change the dimension d()
    friend CerealReader &operator>>(CerealReader &in, QuadraticForm &v);

  private:
    int d_;
    QT w0_;
    std::vector<QT> w1_;
    std::vector< std::vector<QT> > w2_;
};

// Arithmetic
QuadraticForm operator*(const QT &c, const QuadraticForm &f);
