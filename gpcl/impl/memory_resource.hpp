#ifndef GPCL_IMPL_MEMORY_RESOURCE_HPP
#define GPCL_IMPL_MEMORY_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/memory_resource.hpp>

#include <atomic>
#include <new>

namespace gpcl {
inline namespace pmr {

namespace pmr_detail {
class null_memory_resource_impl : public memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    (void)bytes;
    (void)alignment;
    throw std::bad_alloc();
  }

  void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override
  {
    (void)bytes;
    (void)alignment;
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }
};
} // namespace pmr_detail

memory_resource *null_memory_resource() noexcept
{
  static pmr_detail::null_memory_resource_impl instance_;
  return &instance_;
}

namespace pmr_detail {

class new_delete_resource_impl : public memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    return ::operator new[](bytes, std::align_val_t(alignment));
  }
  void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override
  {
    ::operator delete[](p, bytes, std::align_val_t(alignment));
  }
  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }
};

} // namespace pmr_detail

memory_resource *new_delete_resource() noexcept
{
  static pmr_detail::new_delete_resource_impl instance;
  return &instance;
}

namespace pmr_detail {

std::atomic<memory_resource *> &default_memory_resource()
{
  static std::atomic<memory_resource *> instance = pmr::new_delete_resource();
  return instance;
}

} // namespace pmr_detail

void set_default_resource(memory_resource *resource) noexcept
{
  pmr_detail::default_memory_resource().store(resource,
                                              std::memory_order_acq_rel);
}

memory_resource *get_default_resource() noexcept
{
  return pmr_detail::default_memory_resource().load(std::memory_order_acquire);
}

} // namespace pmr
} // namespace gpcl

#endif // GPCL_IMPL_MEMORY_RESOURCE_HPP
