#ifndef GPCL_DECAY_COPY_HPP
#define GPCL_DECAY_COPY_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>

namespace gpcl {

template <typename T>
std::decay_t<T> decay_copy(T &&v)
{
  return detail::forward<T>(v);
}

} // namespace gpcl

#endif // GPCL_DECAY_COPY_HPP
