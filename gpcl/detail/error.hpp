//
// error.hpp
// ~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the GPCL Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.GPCL.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_ERROR_HPP
#define GPCL_DETAIL_ERROR_HPP

#include <gpcl/detail/config.hpp>
#include <iostream>

#if !(defined GPCL_NO_EXCEPTIONS)
#define GPCL_TRY                                                               \
  {                                                                            \
    try
#define GPCL_CATCH(x) catch (x)
#define GPCL_RETHROW throw;
#define GPCL_CATCH_END }
#define GPCL_THROW(x) throw x
#else
#define GPCL_TRY                                                               \
  {                                                                            \
    if (true)
#define GPCL_CATCH(x) else if (false)
#define GPCL_RETHROW
#define GPCL_CATCH_END }
#define GPCL_THROW(x) std::abort()
#endif

#if defined(GPCL_WINDOWS)
#include <Windows.h>
#elif defined(GPCL_POSIX)
#include <cerrno>
#endif

#ifdef GPCL_USE_GPCL_SYSTEM_ERROR

#include <GPCL/system/error_code.hpp>
#include <GPCL/system/system_error.hpp>

namespace gpcl {
namespace detail {

inline namespace errors {

using system_error = GPCL::system::system_error;
using error_code = GPCL::system::error_code;
using GPCL::system::error_category;
using GPCL::system::system_category;
using error_condition = GPCL::system::error_condition;
namespace errc = GPCL::system::errc;
using errc::make_error_code;
using errc::make_error_condition;

} // namespace errors

} // namespace detail
} // namespace gpcl

#else

#include <system_error>

namespace gpcl {
namespace detail {

inline namespace errors {

using system_error = std::system_error;
using error_code = std::error_code;
using std::error_category;
using std::make_error_code;
using std::make_error_condition;
using std::system_category;
using errc = std::errc;
using error_condition = std::error_condition;

} // namespace errors

} // namespace detail
} // namespace gpcl

#endif

namespace gpcl {
namespace detail {

inline namespace errors {
class interrupted : public std::exception
{
public:
  interrupted() = default;
  const char *what() const noexcept final { return "thread interrupted"; }
};
} // namespace errors

[[noreturn]] GPCL_DECL void throw_system_error(int err, const char *what);

[[noreturn]] inline void throw_system_error(const char *what)
{
#if defined(GPCL_WINDOWS)
  throw_system_error(GetLastError(), what);
#elif defined(GPCL_POSIX)
  throw_system_error(errno, what);
#endif
}

template <typename Errc, typename std::enable_if<!std::is_integral<Errc>::value,
                                                 int>::type = 0>
[[noreturn]] inline void throw_system_error(Errc errc, const char *what)
{
  GPCL_THROW(system_error(make_error_code(errc), what));
}

GPCL_DECL void print_error(int err, const char *what) noexcept;
inline void print_error(const char *what) noexcept { print_error(errno, what); }

} // namespace detail
} // namespace gpcl

#if defined(GPCL_HEADER_ONLY)
#include <gpcl/detail/impl/error.ipp>
#endif

#endif
