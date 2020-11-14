#ifndef GPCL_DEFAULT_MALLOC_FREE_USER_ALLOCATOR_HPP
#define GPCL_DEFAULT_MALLOC_FREE_USER_ALLOCATOR_HPP

#include <gpcl/detail/config.hpp>
#include <cstdlib>

namespace gpcl {

struct default_malloc_free_user_allocator
{
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  [[nodiscard]] static char *malloc(size_type sz)
  {
    return static_cast<char *>(std::malloc(sz));
  }

  static void free(void *p) noexcept { std::free(p); }
};

} // namespace gpcl

#endif // GPCL_DEFAULT_MALLOC_FREE_USER_ALLOCATOR_HPP
