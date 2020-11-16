//
// timer.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_TIMER_HPP
#define GPCL_TIMER_HPP

#include <gpcl/assert.hpp>
#include <gpcl/clock.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/posix_timer.hpp>

namespace gpcl {

#ifdef GPCL_POSIX

/// Basic awaitable timer.
template <typename Clock> class basic_timer {
public:
  using clock = Clock;

#if defined(GPCL_POSIX)
  using impl_type = detail::posix_timer<Clock>;
#endif

  basic_timer() = default;

  basic_timer(basic_timer &&) noexcept = default;

  auto operator=(basic_timer &&) noexcept -> basic_timer & = default;

  auto swap(basic_timer &other) noexcept -> void { impl_.swap(other.impl_); }

  friend inline auto swap(basic_timer &x, basic_timer &y) noexcept -> void {
    x.swap(y);
  }

  template <typename Rep1, typename Period1, typename Rep2, typename Period2>
  auto set_time(const chrono::duration<Rep1, Period1> &expires,
      const chrono::duration<Rep2, Period2> &reload) -> void {
    return impl_.set_time(
        chrono::duration_cast<typename clock::duration>(expires),
        chrono::duration_cast<typename clock::duration>(reload));
  }

  template <typename Duration1, typename Rep, typename Period>
  auto set_time(const chrono::time_point<clock, Duration1> &expires,
      const chrono::duration<Rep, Period> &reload) -> void {
    return impl_.set_time(
        chrono::time_point_cast<typename clock::duration>(expires),
        chrono::duration_cast<typename clock::duration>(reload));
  }

  template <typename Clock1, typename Duration1, typename Rep, typename Period>
  auto set_time(const chrono::time_point<Clock1, Duration1> &expires,
      const chrono::duration<Rep, Period> &reload) -> void {
    const auto dur = expires - Clock1::now();
    return impl_.set_time(dur, reload);
  }

  auto get_time() const
      -> std::pair<typename Clock::duration, typename Clock::duration> {
    return impl_.get_time();
  }

  auto wait() -> unsigned int { return impl_.wait(); }

private:
  impl_type impl_;
};
#endif

#if defined(GPCL_POSIX)
using steady_timer = basic_timer<steady_clock>;
#endif

} // namespace gpcl

#endif
