//
// win_condition_variable.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_CONDITION_VARIABLE_HPP
#define GPCL_DETAIL_WIN_CONDITION_VARIABLE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/detail/win_clock.hpp>

#ifdef GPCL_WINDOWS
#include <Windows.h>

namespace gpcl {
namespace detail {

class win_condition_variable : noncopyable {
public:
  using native_handle_type = PCONDITION_VARIABLE;

  GPCL_DECL win_condition_variable() noexcept;

  GPCL_DECL ~win_condition_variable() noexcept;

  GPCL_DECL auto notify_one() -> void;

  GPCL_DECL auto notify_all() -> void;

  GPCL_DECL auto wait(LPCRITICAL_SECTION cs) -> void;

  GPCL_DECL bool wait_until(LPCRITICAL_SECTION cs,
      const chrono::time_point<system_clock> &timeout_time);

  // false: timeout
  // true: no timeout
  GPCL_DECL bool wait_for(
      LPCRITICAL_SECTION cs, const chrono::nanoseconds &rel_time);

  native_handle_type native_handle() { return &cv_; }

private:
  CONDITION_VARIABLE cv_;
};
} // namespace detail
} // namespace gpcl
#endif

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/win_condition_variable.ipp>
#endif

#endif
