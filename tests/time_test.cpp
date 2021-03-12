#include <gpcl/time.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("test duration")
{
  CHECK_THROWS(gpcl::duration::max.checked_add(gpcl::duration::second));
}