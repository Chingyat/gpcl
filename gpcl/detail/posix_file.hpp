//
// posix_file.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_POSIX_FILE_HPP
#define GPCL_DETAIL_POSIX_FILE_HPP

#include <gpcl/creation_tag.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <gpcl/error.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/zstring.hpp>

#include <fcntl.h>

namespace gpcl {
namespace detail {

class posix_file
{
public:
  typedef int native_handle_type;

  enum access_mode
  {
    readonly = O_RDONLY,
    writeonly = O_WRONLY,
    readwrite = O_RDWR,
  };

  posix_file() = default;

  explicit posix_file(native_handle_type fd) : fd_(fd)
  {
    GPCL_ASSERT(fd != -1);
  }

  template <typename CreationTag>
  posix_file(CreationTag, czstring<> filename, access_mode mode)
  {
    open(CreationTag{}, filename, mode);
  }

  posix_file(posix_file &&other) noexcept : fd_(exchange(other.fd_, -1)) {}

  posix_file &operator=(posix_file &&other) noexcept
  {
    swap(other);
    return *this;
  }

  ~posix_file() noexcept { close(); }

  void swap(posix_file &other) noexcept
  {
    using std::swap;
    swap(fd_, other.fd_);
  }

  native_handle_type native_handle() const { return fd_; }

  void assign(native_handle_type fd)
  {
    GPCL_ASSERT(fd_);
    fd_ = fd;
  }

  native_handle_type release() { return exchange(fd_, -1); }

  GPCL_DECL void open(open_only_t, czstring<> filename, access_mode mode,
                      error_code &error);

  GPCL_DECL void open(create_only_t, czstring<> filename, access_mode mode,
                      error_code &error);

  GPCL_DECL void open(open_or_create_t, czstring<> filename, access_mode mode,
                      error_code &error);

  template <typename CreationTag>
  void open(CreationTag tag, czstring<> filename, access_mode mode)
  {
    error_code ec;
    open(tag, filename, mode, ec);
    if (ec)
      GPCL_THROW(system_error(ec, __func__));
  }

  GPCL_DECL void close(error_code &error);

  void close()
  {
    error_code ec;
    close(ec);
    if (ec)
    {
      GPCL_FATAL(ec.value());
    }
  }

  GPCL_DECL posix_file clone(error_code &error);

  posix_file clone()
  {
    error_code ec;
    auto ret = clone(ec);
    if (ec)
    {
      GPCL_FATAL(ec.value());
    }
    return ret;
  }

  bool is_open() const { return fd_ != -1; }

private:
  native_handle_type fd_{-1};
};

GPCL_DECL_INLINE void swap(posix_file &x, posix_file &y) noexcept { x.swap(y); }

} // namespace detail
} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/detail/impl/posix_file.ipp>
#endif // GPCL_HEADER_ONLY

#endif // GPCL_DETAIL_POSIX_FILE_HPP
