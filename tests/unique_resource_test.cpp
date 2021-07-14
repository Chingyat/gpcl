#include <gpcl/unique_resource.hpp>
#include <catch2/catch_test_macros.hpp>
#include <unistd.h>
#include <functional>

TEST_CASE("unique_resource") {
  int fd = ::dup(1);
  {
    gpcl::unique_resource<int, std::function<void (int)>> resource(fd, &::close);

    gpcl::unique_resource<int, std::function<void (int)>> resource2(std::move(resource));
  }

  REQUIRE(::close(fd) != 0);
}
