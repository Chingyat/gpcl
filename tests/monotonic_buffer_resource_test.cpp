#include <gpcl/pmr/monotonic_buffer_resource.hpp>
#include <gpcl/pmr/new_delete_resource.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>
#include <memory>

bool is_aligned(void const *ptr, std::size_t alignment)
{
  REQUIRE(ptr != 0);
  return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

class checking_resource : public gpcl::pmr::memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    auto p = upstream_->allocate(bytes, alignment);
    REQUIRE(allocated_memory_info_
              .emplace(std::piecewise_construct, std::make_tuple(p),
                       std::make_tuple(bytes, alignment))
              .second);
    return p;
  }

  void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override
  {
    REQUIRE_NOTHROW(allocated_memory_info_.at(p) == std::make_tuple(bytes, alignment));
    allocated_memory_info_.erase(p);
    upstream_->deallocate(p, bytes, alignment);
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return this == &other;
  }

  gpcl::pmr::memory_resource *upstream_ = gpcl::pmr::new_delete_resource();
  std::map<void *,
           std::tuple<std::size_t /* sz */, std::size_t /* alignment */>>
      allocated_memory_info_;
};

TEST_CASE("test monotonic_buffer_resource")
{
  char buffer[5];
  checking_resource resource1;
  gpcl::pmr::monotonic_buffer_resource resource(buffer + 1, sizeof buffer, &resource1);

  auto p = resource.allocate(10, 10);
  resource.deallocate(p, 10, 10);

  resource.release();
}
