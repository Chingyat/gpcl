//
// unique_file_descriptor.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/unique_file_descriptor.hpp>

#ifdef GPCL_POSIX
namespace gpcl {
namespace detail {

auto unique_file_descriptor::clone() const -> unique_file_descriptor {
  GPCL_ASSERT(fd_ != -1);
  int fd = ::dup(fd_);
  if (fd == -1)
    throw_system_error("dup");
  return unique_file_descriptor(fd);
}

unique_file_descriptor::~unique_file_descriptor() noexcept {
  if (fd_ != -1)
    GPCL_VERIFY(0 == ::close(fd_));
}

} // namespace detail
} // namespace gpcl

#endif
