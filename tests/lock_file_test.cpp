#include <gpcl/lock_file.hpp>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("file lock") {
  gpcl::lock_file lock_file("test.lock");

  lock_file.lock();

  lock_file.unlock();

  lock_file.lock();

  
}
