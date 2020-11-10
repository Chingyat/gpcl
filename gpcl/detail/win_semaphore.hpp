//
// win_semaphore.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_WIN_SEMAPHORE_HPP
#define GPCL_DETAIL_WIN_SEMAPHORE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/detail/unique_handle.hpp>

#ifdef GPCL_WINDOWS

#include <Windows.h>

namespace gpcl {
namespace detail {

class win_semaphore : noncopyable {
public:
  using value_type = unsigned int;

  GPCL_DECL explicit win_semaphore(value_type init_value);
  GPCL_DECL ~win_semaphore() = default;

  GPCL_DECL auto try_wait() -> bool;
  GPCL_DECL auto wait() -> void;
  GPCL_DECL auto post() -> void;

  using native_handle_type = HANDLE;
  auto native_handle() -> native_handle_type { return sem_.get(); }

private:
  null_handle sem_;
};
} // namespace detail
} // namespace gpcl

#endif

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/win_semaphore.ipp>
#endif

#endif
