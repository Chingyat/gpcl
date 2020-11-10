//
// win_semaphore.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_semaphore.hpp>
#include <limits>

#ifdef GPCL_WINDOWS

namespace gpcl {
namespace detail {

win_semaphore::win_semaphore(value_type init_value)
    : sem_(CreateSemaphoreA(
          nullptr, init_value, (std::numeric_limits<LONG>::max)(), nullptr)) {
  if (!sem_)
    throw_system_error("CreateSemaphoreA");
}

bool win_semaphore::try_wait() {
  switch (WaitForSingleObject(sem_.get(), 0)) {
  case WAIT_OBJECT_0:
    return true;

  case WAIT_TIMEOUT:
    return false;

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

void win_semaphore::wait() {
  switch (WaitForSingleObject(sem_.get(), 0)) {
  case WAIT_OBJECT_0:
    return;

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

void win_semaphore::post() {
  if (!ReleaseSemaphore(sem_.get(), 1, nullptr))
    throw_system_error("ReleaseSemaphore");
}

} // namespace detail
} // namespace gpcl

#endif
