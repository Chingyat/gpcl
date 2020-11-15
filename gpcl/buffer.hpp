//
// buffer.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_BUFFER_HPP
#define GPCL_BUFFER_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/span.hpp>

namespace gpcl {

using const_buffer = span<const char>;
using mutable_buffer = span<char>;

inline const_buffer &operator+=(const_buffer &buf, std::size_t sz)
{
  return buf = buf.subspan(sz);
}

inline mutable_buffer &operator+=(mutable_buffer &buf, std::size_t sz)
{
  return buf = buf.subspan(sz);
}

template <typename T>
const_buffer
buffer(const T &obj,
       std::enable_if_t<std::is_standard_layout<T>::value> * = nullptr)
{
  return const_buffer(reinterpret_cast<const char *>(obj), sizeof obj);
}

inline const_buffer buffer(const void *data, std::size_t size)
{
  return const_buffer(reinterpret_cast<const char *>(data), size);
}

template <typename T>
mutable_buffer
buffer(T &obj, std::enable_if_t<std::is_standard_layout<T>::value> * = nullptr)
{
  return mutable_buffer(reinterpret_cast<char *>(obj), sizeof obj);
}

inline mutable_buffer buffer(void *data, std::size_t size)
{
  return mutable_buffer(reinterpret_cast<char *>(data), size);
}

} // namespace gpcl

#endif // GPCL_BUFFER_HPP
