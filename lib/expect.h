#ifndef EXPECT_H
#define EXPECT_H

#include <cstdio>
#include <cstdlib>

#define expect(x) if (!(x)) { std::printf("Expected: (%s), function %s, file %s, line %d.\n", #x, __PRETTY_FUNCTION__, __FILE__, __LINE__); std::abort(); }

#endif // EXPECT_H
