//
// optional_fwd.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OPTIONAL_FWD_HPP
#define GPCL_OPTIONAL_FWD_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/in_place_t.hpp>
#include <initializer_list>
#include <type_traits>

namespace gpcl {
template <typename T> class optional;

enum class nullopt_t { nullopt };
constexpr nullopt_t nullopt = nullopt_t::nullopt;

// class bad_optional_access
class bad_optional_access;

// relational operators
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator==(
    const optional<T> &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator!=(
    const optional<T> &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator<(
    const optional<T> &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator>(
    const optional<T> &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator<=(
    const optional<T> &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator>=(
    const optional<T> &, const optional<U> &);

template <typename T>
GPCL_DECL_INLINE constexpr bool operator==(
    const optional<T> &, nullopt_t) noexcept;

// comparison with T
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator==(const optional<T> &, const U &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator==(const T &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator!=(const optional<T> &, const U &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator!=(const T &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator<(const optional<T> &, const U &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator<(const T &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator>(const optional<T> &, const U &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator>(const T &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator<=(const optional<T> &, const U &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator<=(const T &, const optional<U> &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator>=(const optional<T> &, const U &);
template <typename T, class U>
GPCL_DECL_INLINE constexpr bool operator>=(const T &, const optional<U> &);

// specialized algorithms
template <typename T> void swap(optional<T> &, optional<T> &) noexcept(true);

template <typename T>
GPCL_DECL_INLINE constexpr optional<typename std::decay<T>::type> make_optional(
    T &&);
template <typename T, class... Args>
GPCL_DECL_INLINE constexpr optional<T> make_optional(Args &&... args);
template <typename T, class U, class... Args>
GPCL_DECL_INLINE constexpr optional<T> make_optional(
    std::initializer_list<U> il, Args &&... args);

// hash support
template <typename T> struct hash;
template <typename T> struct hash<optional<T>>;

} // namespace gpcl

#endif // GPCL_OPTIONAL_FWD_HPP
