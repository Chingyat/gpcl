//
// narrow_cast.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_NARROW_CAST_HPP
#define GPCL_NARROW_CAST_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <exception>
#include <type_traits>

namespace gpcl {

template <typename To, typename From>
To narrow_cast(From F) noexcept
{
  static_assert(std::is_integral<From>() && std::is_integral<To>(),
                "Both From and To must be integral type");

  To T = static_cast<To>(F);
  GPCL_ASSERT(static_cast<From>(T) == F);

  return T;
}

class bad_narrow_cast : public std::exception
{
public:
  bad_narrow_cast() = default;

  const char *what() const noexcept final { return "bad_narrow_cast"; }
};

template <typename To, typename From>
To narrow(From F) noexcept
{
  static_assert(std::is_integral<From>() && std::is_integral<To>(),
                "Both From and To must be integral type");

  To T = static_cast<To>(F);
  if (static_cast<From>(T) != F)
    throw bad_narrow_cast();

  return T;
}

} // namespace gpcl

#endif
