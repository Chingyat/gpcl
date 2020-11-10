//
// win_condition_variable.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_condition_variable.hpp>
#include <gpcl/detail/win_mutex.hpp>
#include <gpcl/narrow_cast.hpp>

#ifdef GPCL_WINDOWS
#include <Windows.h>

extern "C" {
WINBASEAPI void __stdcall InitializeConditionVariable(CONDITION_VARIABLE *);
WINBASEAPI void __stdcall WakeConditionVariable(CONDITION_VARIABLE *);
WINBASEAPI void __stdcall WakeAllConditionVariable(CONDITION_VARIABLE *);
WINBASEAPI BOOL __stdcall SleepConditionVariableCS(
    CONDITION_VARIABLE *, CRITICAL_SECTION *, DWORD);
}

namespace gpcl {
namespace detail {

win_condition_variable::win_condition_variable() noexcept {
  ::InitializeConditionVariable(&cv_);
}

win_condition_variable::~win_condition_variable() noexcept {}

auto win_condition_variable::notify_one() -> void {
  ::WakeConditionVariable(&cv_);
}

auto win_condition_variable::notify_all() -> void {
  ::WakeAllConditionVariable(&cv_);
}

auto win_condition_variable::wait(LPCRITICAL_SECTION cs) -> void {
  if (!::SleepConditionVariableCS(&cv_, cs, INFINITE))
    throw_system_error("SleepConditionVariableCS");
}

bool win_condition_variable::wait_until(LPCRITICAL_SECTION cs,
    const chrono::time_point<system_clock> &timeout_time) {
  auto rel_time = timeout_time - system_clock::now();
  if (rel_time.count() < 0) {
    rel_time = decltype(rel_time)(0);
  }

  return wait_for(cs, rel_time);
}

bool win_condition_variable::wait_for(
    LPCRITICAL_SECTION cs, const chrono::nanoseconds &rel_time) {
  const auto ms = chrono::duration_cast<chrono::milliseconds>(rel_time).count();
  if (!::SleepConditionVariableCS(&cv_, cs, narrow_cast<DWORD>(ms))) {
    DWORD dwError = ::GetLastError();
    if (dwError == ERROR_TIMEOUT)
      return true;
    throw_system_error(dwError, "SleepConditionVariableCS");
  }

  return false;
}

} // namespace detail
} // namespace gpcl

#endif
