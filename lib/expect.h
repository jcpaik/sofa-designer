#ifndef EXPECT_H
#define EXPECT_H

#include <cstdio>
#include <cstdlib>

#define expect(x, reason) if (!(x)) { std::printf("%s: (%s), function %s, file %s, line %d.\n", reason, #x, __PRETTY_FUNCTION__, __FILE__, __LINE__); std::abort(); }

#endif // EXPECT_H
