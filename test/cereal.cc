#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include <iostream>
#include <unistd.h>

#include "sofa/context.h"
#include "sofa/cereal.h"
#include "sofa/qp.h"
#include "sofa/geom.h"

void debug_states(std::vector<SofaState> valid_states) {
  
  std::cout << valid_states.size() << " states" << std::endl;
  for (auto &s : valid_states) {
    std::cout << "area " << s.area() << std::endl;
    for (auto ee : s.e())
      std::cout << ee << " ";
    std::cout << std::endl;
    std::cout << "{" << std::endl;
    for (int i = 0; i <= int(s.e().size()) - 2; i++) {
      std::cout << '{' << s.v(i).x << ", " << s.v(i).y << "}, " << std::endl;
    }
    std::cout << "}" << std::endl;
  }
}

TEST_CASE( "Checking exactness of cereal read/write", "[CEREAL]" ) {
  {
    CerealWriter w("size_t.crl");
    w << size_t(10003040);
    w.close();

    CerealReader r("size_t.crl");
    size_t b;
    r >> b;
    r.close();

    REQUIRE( b == size_t(10003040) );
  }
  {
    CerealWriter w("bignum.crl");
    NT a("1980720387507239059028309589023890535423454321"), b;
    w << a;
    w.close();

    CerealReader r("bignum.crl");
    r >> b;
    r.close();

    REQUIRE( a == b );
  }
  {
    CerealWriter w("bigrat.crl");
    QT a{NT("198072038750723905902"),NT("309589023890535423454321")}, b;
    w << a;
    w.close();

    CerealReader r("bigrat.crl");
    r >> b;
    r.close();

    REQUIRE( a == b );
  }
  {
    save("vector.crl", Vector(QT{1, 2}, QT{3, 4}));
    Vector v;
    load("vector.crl", v);
  }
  {
    std::vector<Vector> v(3, Vector(QT{1, 2}, QT{3, 4}));
    save("vecs.crl", v);
    std::vector<Vector> vv;
    load("vecs.crl", vv);
    REQUIRE (v == vv);
  }
  {
    CerealWriter w("biglform.crl");
    LinearForm a(1, {2, 3, 4}), b;
    w << a;
    w.close();

    CerealReader r("biglform.crl");
    r >> b;
    r.close();

    REQUIRE( a == b );
    REQUIRE( b.w0() == 1 );
    REQUIRE( b.w1(0) == 2 );
    REQUIRE( b.w1(1) == 3 );
    REQUIRE( b.w1(2) == 4 );
  }
  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      } 
      );
  {
    QuadraticForm a(ctx.area({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0})); 
    CerealWriter w("bigqform.crl");
    w << a;
    w.close();

    QuadraticForm b(2);
    REQUIRE( a != b );

    CerealReader r("bigqform.crl");
    r >> b;
    r.close();

    REQUIRE( a == b );
  }
  {
    save("ctx.crl", ctx);
    SofaContext cc("ctx.crl");
    auto v = sofa_area_qp(
        cc.area({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0}), 
        cc,
        cc.default_constraints()).optimality_proof().max_area;
    NT a("858413240226912435793494170653955337517692586819527086695188");
    NT b("347534368299812191344928277446711705522020432192595617308389");
    QT q(a, b);
    REQUIRE(v == q);
  }
  {
    SofaContext ctx({
        {QT{1911,1961},QT{440,1961}},
        {QT{85608,95017},QT{41225,95017}},
        {QT{351,449},QT{280,449}},
        {QT{280,449},QT{351,449}},
        {QT{41225,95017},QT{85608,95017}},
        {QT{440,1961},QT{1911,1961}}
        });
    SofaBranchTree t(ctx);
    t.add_corner(3);
    t.add_corner(4);
    // check if they give the same result
    debug_states(t.valid_states());
    save("tree.crl", t);

    SofaBranchTree t2(ctx, "tree.crl");
    // check if they give the same result
    debug_states(t2.valid_states());
  }
  /*
  BENCHMARK("qform store and write") {
    QuadraticForm a(ctx.area({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0})); 
    CerealWriter w("bigqform.crl");
    w << a;
    w.close();

    QuadraticForm b(2);
    REQUIRE( a != b );

    CerealReader r("bigqform.crl");
    r >> b;
    r.close();

    REQUIRE( a == b );
  };
  */
}
