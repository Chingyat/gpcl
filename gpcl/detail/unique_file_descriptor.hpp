//
// unique_file_descriptor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef RTXX_DETAIL_UNIQUE_FILE_DESCRIPTOR
#define RTXX_DETAIL_UNIQUE_FILE_DESCRIPTOR

#include <gpcl/detail/config.hpp>
#include <utility>

#ifdef GPCL_POSIX
#include <unistd.h>

namespace gpcl {
namespace detail {

class unique_file_descriptor {
public:
  explicit unique_file_descriptor(int fd = -1) noexcept : fd_(fd) {}

  unique_file_descriptor(unique_file_descriptor &&other) noexcept
      : unique_file_descriptor(other.release()) {}

  unique_file_descriptor &operator=(unique_file_descriptor &&other) noexcept {
    swap(other);
    return *this;
  }

  GPCL_DECL ~unique_file_descriptor() noexcept;

  void swap(unique_file_descriptor &other) noexcept {
    using std::swap;
    swap(fd_, other.fd_);
  }

  friend void swap(
      unique_file_descriptor &x, unique_file_descriptor &y) noexcept {
    x.swap(y);
  }

  friend bool operator<(const unique_file_descriptor &x,
      const unique_file_descriptor &y) noexcept {
    return x.get() < y.get();
  }

  [[nodiscard]] GPCL_DECL auto clone() const -> unique_file_descriptor;

  [[nodiscard]] int get() const noexcept { return fd_; }

  explicit operator bool() const noexcept { return fd_ != -1; }

  int release() noexcept {
    int ret = get();
    fd_ = -1;
    return ret;
  }

  void reset(int fd = -1) noexcept { unique_file_descriptor(fd).swap(*this); }

private:
  int fd_{-1};
};

} // namespace detail
} // namespace gpcl

#endif

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/unique_file_descriptor.ipp>
#endif

#endif
