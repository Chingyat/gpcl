//
// assert.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_ASSERT_HPP
#define GPCL_ASSERT_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/unreachable.hpp>

#ifdef GPCL_USE_BOOST_ASSERT
#include <boost/assert.hpp>
#define GPCL_ASSERT BOOST_ASSERT
#else

#ifndef GPCL_WINDOWS
#include <cassert>
#define GPCL_ASSERT assert
#else
#include <crtdbg.h>
#define GPCL_ASSERT _ASSERT
#endif

#endif

/// \entity GPCL_ASSERT
/// General purpose assertion

/// Assertion used to verify postconditions
#define GPCL_VERIFY(...) \
  do { \
    [[maybe_unused]] auto _ = static_cast<bool>(__VA_ARGS__); \
    GPCL_ASSERT(_ &&#__VA_ARGS__); \
    (void)_; \
  } while (false)

#define GPCL_VERIFY_(expected, expr) GPCL_VERIFY(expected == (expr))

#define GPCL_VERIFY_0(expr) GPCL_VERIFY_(0, expr)

/// Used to assert that the line cannot be reached
#define GPCL_UNREACHABLE(msg) \
  ::gpcl::detail::unreachable_internal(msg, __FILE__, __LINE__)

/// Indicates that the function is not implemented yet
#define GPCL_UNIMPLEMENTED() GPCL_UNREACHABLE("unimplemented")

#endif
