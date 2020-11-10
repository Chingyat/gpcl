//
// expected_fwd.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_UNEXPECTED_FWD_HPP
#define GPCL_UNEXPECTED_FWD_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>

namespace gpcl {

template <typename E> class unexpected;

template <typename T, typename E> class [[nodiscard]] expected;

template <typename E> class bad_expected_access;

template <> class bad_expected_access<void>;

template <typename E, std::enable_if_t<std::is_swappable_v<E>, int> = 0>
GPCL_DECL_INLINE void swap(unexpected<E> &lhs, unexpected<E> &rhs) noexcept(
    std::is_nothrow_swappable_v<E>);

template <typename E1, typename E2>
GPCL_DECL_INLINE constexpr bool operator==(
    const unexpected<E1> &x, const unexpected<E2> &y);

template <typename E1, typename E2>
GPCL_DECL_INLINE constexpr bool operator!=(
    const unexpected<E1> &x, const unexpected<E2> &y);

} // namespace gpcl

#endif
