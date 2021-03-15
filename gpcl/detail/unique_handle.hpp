//
// unique_handle.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#include <gpcl/detail/config.hpp>

#ifdef GPCL_WINDOWS
#include <windows.h>

namespace gpcl {

namespace detail {

template <typename Traits> class unique_handle {
  typename Traits::pointer p_;

  auto close() noexcept -> void {
    if (p_ != Traits::invalid())
      Traits::close(p_);
    p_ = Traits::invalid();
  }

public:
  using pointer = typename Traits::pointer;

  unique_handle(const unique_handle &) = delete;
  auto operator=(unique_handle const &) -> unique_handle & = delete;

  explicit unique_handle(pointer h = Traits::invalid()) noexcept : p_(h) {}

  unique_handle(unique_handle &&other) noexcept
      : unique_handle(other.release()) {}

  auto operator=(unique_handle &&other) noexcept -> unique_handle & {
    swap(other);
    return *this;
  }

  auto swap(unique_handle &other) noexcept -> void {
    using std::swap;
    swap(p_, other.p_);
  }

  friend inline auto swap(unique_handle &x, unique_handle &y) noexcept -> void {
    x.swap(y);
  }

  ~unique_handle() { close(); }

  constexpr auto get() const noexcept -> pointer { return p_; }

  constexpr auto release() noexcept -> pointer {
    auto r = p_;
    p_ = Traits::invalid();
    return r;
  }

  auto reset(pointer h = Traits::invalid()) noexcept -> void {
    close();
    p_ = h;
  }

  explicit constexpr operator bool() const noexcept {
    return p_ != Traits::invalid();
  }
};

struct invalid_handle_traits {
  using pointer = ::HANDLE;

  static auto invalid() noexcept -> pointer { return INVALID_HANDLE_VALUE; }

  GPCL_DECL static auto close(pointer h) noexcept -> void;
};

struct null_handle_traits {
  using pointer = ::HANDLE;

  static constexpr auto invalid() noexcept -> ::HANDLE { return nullptr; }

  GPCL_DECL static auto close(::HANDLE h) noexcept -> void;
};

using invalid_handle = unique_handle<invalid_handle_traits>;

using null_handle = unique_handle<null_handle_traits>;

} // namespace detail

} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/unique_handle.ipp>
#endif

#endif
