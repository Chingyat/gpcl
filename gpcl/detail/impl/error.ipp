//
// error.ipp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdio>  // for fprintf
#include <cstring> // for strerror
#include <gpcl/assert.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/error.hpp>

#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
#include <boost/throw_exception.hpp>
#endif

namespace gpcl {
namespace detail {

void throw_system_error(int err, const char *what) {
  if (err == EINTR) {
    GPCL_THROW(interrupted());
  }

#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
  BOOST_THROW_EXCEPTION(system_error(err, system_category(), what));
#else
  GPCL_THROW(system_error(err, system_category(), what));
#endif

  GPCL_UNREACHABLE("should not return");
}

void print_error(int err, const char *what) noexcept {
#ifndef GPCL_WINDOWS
  std::fprintf(stderr, "%s: %s", what, std::strerror(err));
#else
  std::fprintf(stderr, "%s: %s", what, system_category().message(err).c_str());
#endif
}

} // namespace detail
} // namespace gpcl
