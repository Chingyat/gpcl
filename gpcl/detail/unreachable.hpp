//
// unreachable.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_UNREACHABLE_HPP
#define GPCL_DETAIL_UNREACHABLE_HPP

#include <gpcl/detail/config.hpp>

namespace gpcl { namespace detail {

[[noreturn]] GPCL_DECL void unreachable_internal(const char *msg = nullptr,
                                                 const char *file = nullptr,
                                                 unsigned line = 0) noexcept;

} } // namespace gpcl::detail

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/unreachable.ipp>
#endif

#endif
