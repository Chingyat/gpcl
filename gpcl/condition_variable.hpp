//
// condition_variable.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CONDITION_VARIABLE_HPP
#define GPCL_CONDITION_VARIABLE_HPP

#include <gpcl/assert.hpp>
#include <gpcl/clock.hpp>
#include <gpcl/detail/posix_condition_variable.hpp>
#include <gpcl/detail/win_condition_variable.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/unique_lock.hpp>
#include <type_traits>

namespace gpcl {

#if defined(GPCL_POSIX) || defined(GPCL_WINDOWS)

enum class cv_status : bool { no_timeout, timeout };

class condition_variable {
public:
#if defined(GPCL_POSIX)
  using impl_type = detail::posix_condition_variable;
#elif defined(GPCL_WINDOWS)
  using impl_type = detail::win_condition_variable;
#endif

  condition_variable() = default;

  condition_variable(const condition_variable &) = delete;
  condition_variable(condition_variable &&) = delete;

  condition_variable &operator=(const condition_variable &) = delete;
  auto operator=(condition_variable &&) -> condition_variable & = delete;

  void wait(unique_lock<mutex> &lock) {
    GPCL_ASSERT(lock.owns_lock());
    impl_.wait(lock.mutex()->native_handle());
  }

  template <typename Predicate>
  void wait(unique_lock<mutex> &lock, Predicate pred) {
    while (!pred()) {
      wait(lock);
    }
  }

  template <typename Clock, typename Duration,
      typename std::enable_if<std::is_same<Clock, system_clock>::value,
          int>::type = 0>
  cv_status wait_until(unique_lock<mutex> &lock,
      chrono::time_point<Clock, Duration> const &timeout_time) {
    GPCL_ASSERT(lock.owns_lock());
    return static_cast<cv_status>(impl_.wait_until(lock, timeout_time));
  }

  template <typename Clock, typename Duration,
      typename std::enable_if<!std::is_same<Clock, system_clock>::value,
          int>::type = 0>
  cv_status wait_until(unique_lock<mutex> &lock,
      chrono::time_point<Clock, Duration> const &timeout_time) {
    GPCL_ASSERT(lock.owns_lock());
    return static_cast<cv_status>(
        impl_.wait_for(lock, timeout_time - Clock::now()));
  }

  template <typename Rep, typename Period>
  cv_status wait_for(
      unique_lock<mutex> &lock, const chrono::duration<Rep, Period> &rel_time) {
    GPCL_ASSERT(lock.owns_lock());
    return static_cast<cv_status>(impl_.wait_for(lock.mutex()->native_handle(),
        chrono::duration_cast<chrono::nanoseconds>(rel_time)));
  }

  template <typename Clock, typename Duration, typename Predicate>
  bool wait_until(unique_lock<mutex> &lock,
      chrono::time_point<Clock, Duration> &timeout_time, Predicate pred) {
    while (!pred()) {
      if (wait_until(lock, timeout_time) == cv_status::timeout)
        return pred();
    }
    GPCL_VERIFY(lock.owns_lock());
    return true;
  }

  template <typename Rep, typename Period, typename Predicate>
  bool wait_for(unique_lock<mutex> &lock,
      chrono::duration<Rep, Period> const &rel_time, Predicate pred) {
    while (!pred()) {
      if (wait_for(lock, rel_time) == cv_status::timeout)
        return pred();
    }
    GPCL_VERIFY(lock.owns_lock());
    return true;
  }

  void notify_one() { impl_.notify_one(); }

  void notify_all() { impl_.notify_all(); }

private:
  impl_type impl_;
};

#endif

} // namespace gpcl

#if defined(GPCL_HEADER_ONLY)
#include <gpcl/detail/posix_condition_variable.hpp>
#endif

#endif
