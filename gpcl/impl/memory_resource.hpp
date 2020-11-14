#ifndef GPCL_IMPL_MEMORY_RESOURCE_HPP
#define GPCL_IMPL_MEMORY_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/memory_resource.hpp>

#include <atomic>

namespace gpcl {

namespace detail {
class null_memory_resource_impl : public memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    (void)bytes;
    (void)alignment;
    throw std::bad_alloc();
  }

  void do_deallocate(void *p, std::size_t bytes, std::size_t aligement) override
  {
    (void)bytes;
    (void)aligement;
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }
};
} // namespace detail

memory_resource *null_memory_resource() noexcept
{
  static detail::null_memory_resource_impl instance_;
  return &instance_;
}

namespace detail {

class new_delete_resource_impl : public memory_resource
{
  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    return ::operator new[](bytes, std::align_val_t(alignment));
  }
  void do_deallocate(void *p, std::size_t bytes, std::size_t aligement) override
  {
    ::operator delete[](p, bytes);
  }
  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }
};

} // namespace detail

memory_resource *new_delete_resource() noexcept
{
  static detail::new_delete_resource_impl instance;
  return &instance;
}

namespace detail {

std::atomic<memory_resource *> &default_memory_resource()
{
  static std::atomic<memory_resource *> instance = new_delete_resource();
  return instance;
}

} // namespace detail

void set_memory_resource(memory_resource *resource) noexcept
{
  detail::default_memory_resource() = resource;
}

memory_resource *get_memory_resource() noexcept
{
  return detail::default_memory_resource();
}

} // namespace gpcl

#endif // GPCL_IMPL_MEMORY_RESOURCE_HPP
