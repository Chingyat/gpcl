#ifndef GPCL_MEMORY_RESOURCE_HPP
#define GPCL_MEMORY_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <new>

namespace gpcl {

class memory_resource
{
public:
  virtual ~memory_resource() = default;

  [[nodiscard]] void *
  allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    return do_allocate(bytes, alignment);
  }

  void deallocate(void *p, std::size_t bytes,
                  std::size_t alignment = alignof(std::max_align_t))
  {
    return do_deallocate(p, bytes, alignment);
  }

  bool is_equal(const memory_resource &other) const noexcept
  {
    return do_is_equal(other);
  }

private:
  virtual void *do_allocate(std::size_t bytes, std::size_t alignment) = 0;

  virtual void do_deallocate(void *p, std::size_t bytes,
                             std::size_t aligement) = 0;

  virtual bool do_is_equal(const memory_resource &other) const noexcept = 0;
};

GPCL_DECL memory_resource *new_delete_resource() noexcept;

GPCL_DECL memory_resource *null_memory_resource() noexcept;

GPCL_DECL memory_resource *get_default_resource() noexcept;

GPCL_DECL void set_default_resource(memory_resource *resource) noexcept;

class monotonic_buffer_resource;


} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/impl/memory_resource.hpp>
#endif

#endif // GPCL_MEMORY_RESOURCE_HPP
