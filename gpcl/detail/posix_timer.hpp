//
// posix_timer.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_POSIX_TIMER_HPP
#define GPCL_POSIX_TIMER_HPP

#include <gpcl/assert.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/detail/posix_clock.hpp>
#include <gpcl/detail/unique_file_descriptor.hpp>

#ifdef GPCL_POSIX
#include <sys/timerfd.h>

namespace gpcl {
namespace detail {

class posix_timer_base {
public:
  GPCL_DECL explicit posix_timer_base(::clockid_t clock);

  posix_timer_base(posix_timer_base &&) noexcept = default;
  posix_timer_base &operator=(posix_timer_base &&) noexcept = default;

  auto swap(posix_timer_base &other) noexcept -> void {
    timerfd_.swap(other.timerfd_);
  }

  friend inline auto swap(posix_timer_base &x, posix_timer_base &y) noexcept
      -> void {
    x.swap(y);
  }

  GPCL_DECL void set_time_abs(const struct itimerspec *its);
  GPCL_DECL void set_time_rel(const struct itimerspec *its);

  GPCL_DECL void get_time_rel(struct itimerspec *its) const;

  // wait for the timer to expires, returns the expires count
  GPCL_DECL unsigned int wait();

  using native_handle_type = int;

  [[nodiscard]] native_handle_type native_handle() const {
    return timerfd_.get();
  }

protected:
  ~posix_timer_base() = default;

  unique_file_descriptor timerfd_;
};

template <typename PosixClock> class posix_timer : public posix_timer_base {
public:
  using clock = PosixClock;
  using time_point = typename clock::time_point;
  using duration = typename clock::duration;

  posix_timer() : posix_timer_base(clock::clock_id) {}

  void set_time(const duration &expires, const duration &reload);

  void set_time(const time_point &expires, const duration &reload);

  std::pair<duration, duration> get_time() const;
};

template <typename PosixClock>
void posix_timer<PosixClock>::set_time(
    const duration &expires, const duration &reload) {
  if (expires.count() > 0) {
    auto its = itimerspec{};
    its.it_value = to_timespec(expires);
    its.it_interval = to_timespec(reload);
    set_time_rel(&its);
  } else {
    auto its = itimerspec{};
    its.it_value.tv_nsec = 1;
    its.it_interval = to_timespec(reload);
    set_time_rel(&its);
  }
}

template <typename PosixClock>
void posix_timer<PosixClock>::set_time(
    const time_point &expires, const duration &reload) {
  auto its = itimerspec{};
  its.it_value = to_timespec(expires.time_since_epoch());
  its.it_interval = to_timespec(reload);
  set_time_abs(&its);
}

template <typename PosixClock>
std::pair<typename PosixClock::duration, typename PosixClock::duration>
posix_timer<PosixClock>::get_time() const {
  auto its = itimerspec{};
  posix_timer_base::get_time_rel(&its);

  return std::make_pair(
      to_duration(its.it_value), to_duration(its.it_interval));
}

} // namespace detail
} // namespace gpcl
#endif

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/posix_timer.ipp>
#endif

#endif
