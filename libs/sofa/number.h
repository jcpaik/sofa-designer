#pragma once

#include <CGAL/gmpxx.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Quotient.h>

typedef mpz_class NT;
// typedef CGAL::Gmpz NT;

template<> inline void CGAL::simplify_quotient<NT>(NT &a, NT &b) {
  NT g = gcd(a, b);
  a /= g; b /= g;
}

typedef CGAL::Quotient<NT> QT;
