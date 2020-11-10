//
// type_traits.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_TYPE_TRAITS_HPP
#define GPCL_DETAIL_TYPE_TRAITS_HPP

#include <gpcl/detail/config.hpp>
#include <type_traits>

namespace gpcl {
namespace detail {

#define GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(name_part) \
  template <typename T, bool = std::is_void<T>::value> struct name_part; \
  template <typename T> struct name_part<T, true> : std::false_type {}; \
  template <typename T> struct name_part<T, false> : std::name_part<T> {}; \
  template <typename T> \
  GPCL_CXX17_INLINE_CONSTEXPR bool name_part##_v = name_part<T>{};

GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_default_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_default_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_default_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_copy_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_copy_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_copy_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_move_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_move_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_move_constructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_copy_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_copy_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_copy_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_move_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_move_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_nothrow_move_assignable)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_destructible)
GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE(is_trivially_destructible)

#undef GPCL_TYPE_TRAITS_TREAT_VOID_AS_FALSE

template <typename T> struct is_char_like_type : std::false_type {};

template <> struct is_char_like_type<char> : std::true_type {};

template <> struct is_char_like_type<unsigned char> : std::true_type {};

template <> struct is_char_like_type<signed char> : std::true_type {};

template <> struct is_char_like_type<wchar_t> : std::true_type {};

template <> struct is_char_like_type<char16_t> : std::true_type {};

template <> struct is_char_like_type<char32_t> : std::true_type {};

template <typename T>
GPCL_CXX17_INLINE_CONSTEXPR bool is_char_like_type_v = is_char_like_type<T>{};

template <typename T, typename... Args>
GPCL_CXX17_INLINE_CONSTEXPR bool is_constructible_v =
    std::is_constructible<T, Args...>{};

template <typename T, typename... Args>
GPCL_CXX17_INLINE_CONSTEXPR bool is_nothrow_constructible_v =
    std::is_nothrow_constructible<T, Args...>{};

template <typename T>
GPCL_CXX17_INLINE_CONSTEXPR bool is_void_v = std::is_void<T>{};

template <typename T, typename U>
GPCL_CXX17_INLINE_CONSTEXPR bool is_same_v = std::is_same<T, U>{};

template <typename From, typename To>
GPCL_CXX17_INLINE_CONSTEXPR bool is_convertible_v =
    std::is_convertible<From, To>{};

template <typename T, typename U>
GPCL_CXX17_INLINE_CONSTEXPR bool is_assignable_v = std::is_assignable<T, U>{};

} // namespace detail

} // namespace gpcl

#endif // GPCL_TYPE_TRAITS_HPP
