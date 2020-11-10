//
// posix_timer.ipp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/error.hpp>
#include <gpcl/detail/posix_timer.hpp>
#include <gpcl/detail/unique_file_descriptor.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/narrow_cast.hpp>

#ifdef GPCL_POSIX
#include <sys/timerfd.h>

namespace gpcl {
namespace detail {

posix_timer_base::posix_timer_base(::clockid_t clock) {
  GPCL_ASSERT(clock == CLOCK_MONOTONIC || clock == CLOCK_REALTIME ||
              clock == CLOCK_BOOTTIME || clock == CLOCK_REALTIME_ALARM ||
              clock == CLOCK_BOOTTIME_ALARM);

  unique_file_descriptor fd(::timerfd_create(clock, TFD_CLOEXEC));
  if (!fd) {
    throw_system_error("timerfd_create");
  }

  timerfd_ = detail::move(fd);
}

void posix_timer_base::set_time_abs(const struct itimerspec *ts) {
  int r = ::timerfd_settime(timerfd_.get(), TFD_TIMER_ABSTIME, ts, nullptr);
  if (r)
    throw_system_error("timerfd_settime");
}

void posix_timer_base::set_time_rel(const struct itimerspec *ts) {
  int r = ::timerfd_settime(timerfd_.get(), 0, ts, nullptr);
  if (r)
    throw_system_error("timerfd_settime");
}

void posix_timer_base::get_time_rel(struct itimerspec *ts) const {
  int r = ::timerfd_gettime(timerfd_.get(), ts);
  if (r)
    throw_system_error("timerfd_gettime");
}

unsigned int posix_timer_base::wait() {
  uint64_t buf;
  int r = ::read(timerfd_.get(), &buf, sizeof(buf));
  if (r < 0)
    throw_system_error("read");
  return narrow_cast<unsigned int>(buf);
}

} // namespace detail
} // namespace gpcl

#endif
