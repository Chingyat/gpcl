//
// is_basic_lockable.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IS_BASIC_LOCKABLE_HPP
#define GPCL_IS_BASIC_LOCKABLE_HPP

#include <gpcl/detail/config.hpp>
#include <type_traits>

namespace gpcl {

template <typename T, typename = void>
struct is_basic_lockable : std::false_type {};

template <typename T>
struct is_basic_lockable<T,
    decltype(std::declval<T>().lock(), std::declval<T>().unlock(), void())>
    : std::true_type {};

} // namespace gpcl

#endif // GPCL_IS_BASIC_LOCKABLE_HPP
