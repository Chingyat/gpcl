//
// semaphore.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SEMAPHORE_HPP
#define GPCL_SEMAPHORE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_semaphore.hpp>
#include <gpcl/detail/win_semaphore.hpp>
#include <gpcl/noncopyable.hpp>

namespace gpcl {

#if defined(GPCL_POSIX) || defined(GPCL_WINDOWS)

/// Semaphore.
class semaphore : noncopyable {
public:
#if defined(GPCL_POSIX)
  using impl_type = gpcl::detail::posix_semaphore;
#elif defined(GPCL_WINDOWS)
  using impl_type = gpcl::detail::win_semaphore;
#endif
  using value_type = impl_type::value_type;
  using native_handle_type = impl_type::native_handle_type;

  /// Constructor.
  explicit semaphore(
      value_type init_value = 0)
      : impl_(init_value)
  {
  }

  /// Disable Copy.
  semaphore(const semaphore &) = delete;
  auto operator=(const semaphore &) -> semaphore & = delete;
  semaphore(semaphore &&) = delete;
  auto operator=(semaphore &&) -> semaphore & = delete;

  /// Unlock a semaphore.
  auto post() -> void
  {
    impl_.post();
  }

  /// Lock a semaphore.
  auto wait() -> void
  {
    impl_.wait();
  }

  /// Lock a semaphore.
  auto try_wait() -> bool
  {
    return impl_.try_wait();
  }

  /// Returns a native handle of the semaphore.
  auto native_handle() -> native_handle_type
  {
    return impl_.native_handle();
  }

private:
  impl_type impl_;
};
#endif

} // namespace gpcl

#endif
