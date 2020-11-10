//
// offset_ptr.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_OFFSET_PTR_HPP
#define GPCL_IMPL_OFFSET_PTR_HPP

#include <gpcl/narrow_cast.hpp>
#include <gpcl/offset_ptr.hpp>

namespace gpcl {

template <typename T>
offset_ptr<T>::offset_ptr(T *ptr)
    : offset_{ptr ? narrow_cast<offset_type>(
                        detail::as_integer(ptr) - detail::as_integer(this))
                  : poison} {}

template <typename T>
constexpr offset_ptr<T> &offset_ptr<T>::operator=(const offset_ptr &other) {
  const auto ptr = other._real_ptr();
  offset_ =
      ptr ? narrow_cast<offset_type>(reinterpret_cast<std::intptr_t>(ptr) -
                                     reinterpret_cast<std::intptr_t>(this))
          : poison;
  return *this;
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> offset_ptr<T>::operator--(int) & {
  GPCL_ASSERT(_real_ptr() != 0 && "decrementing a null pointer");

  const offset_ptr ret = *this;
  --*this;
  return ret;
}
template <typename T>

constexpr typename offset_ptr<T>::pointer offset_ptr<T>::_real_ptr() const {
  if (!std::is_void<value_type>::value &&
      !std::is_same<value_type, char>::value &&
      !std::is_same<value_type, signed char>::value &&
      !std::is_same<value_type, unsigned char>::value
#if __cplusplus > 201703L
      && !std::is_same<value_type, std::byte>::value
#endif
  ) {
    GPCL_ASSERT(offset_ != 0 && "maybe using uninitialized value");
  }
  return offset_ != poison
             ? reinterpret_cast<pointer>(detail::as_integer(this) + offset_)
             : nullptr;
}

template <typename T>
constexpr typename offset_ptr<T>::offset_type offset_ptr<T>::diff_to_offset(
    offset_ptr::difference_type diff) {
  return narrow_cast<offset_type>(diff * sizeof(T));
}

template <typename T>
constexpr typename offset_ptr<T>::difference_type offset_ptr<T>::offset_to_diff(
    offset_ptr::offset_type offset) {
  GPCL_ASSERT(offset % sizeof(T) == 0);
  return static_cast<difference_type>(offset) / sizeof(T);
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value, int>::type>
auto &offset_ptr<T>::operator*() const {
  GPCL_ASSERT(_real_ptr() != 0);
  return *_real_ptr();
}

template <typename T>
typename offset_ptr<T>::pointer offset_ptr<T>::operator->() const {
  GPCL_ASSERT(_real_ptr() && "dereferencing a null pointer");
  return _real_ptr();
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> &offset_ptr<T>::operator++() & {
  GPCL_ASSERT(_real_ptr() != 0 && "incrementing a null pointer");
  offset_ += sizeof(T);
  return *this;
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
const offset_ptr<T> offset_ptr<T>::operator++(int) & {
  GPCL_ASSERT(_real_ptr() != 0 && "incrementing a null pointer");
  const auto ret = *this;
  ++*this;
  return ret;
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> &offset_ptr<T>::operator--() & {
  GPCL_ASSERT(_real_ptr() != 0 && "decrementing a null pointer");

  offset_ -= sizeof(T);
  return *this;
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> &offset_ptr<T>::operator+=(offset_ptr::difference_type diff) {
  GPCL_ASSERT(_real_ptr() != 0);

  offset_ += diff_to_offset(diff);
  return *this;
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> &offset_ptr<T>::operator-=(offset_ptr::difference_type diff) {
  GPCL_ASSERT(_real_ptr() != 0);

  offset_ -= diff_to_offset(diff);
  return *this;
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
typename offset_ptr<T>::difference_type offset_ptr<T>::operator-(
    const offset_ptr &other) const {
  GPCL_ASSERT(_real_ptr() != 0);
  GPCL_ASSERT(other._real_ptr() != 0);

  return _real_ptr() - other._real_ptr();
}

template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> offset_ptr<T>::operator+(offset_ptr::difference_type diff) const {
  GPCL_ASSERT(_real_ptr() != 0);

  offset_ptr ret = *this;
  ret += diff;
  return ret;
}
template <typename T>
template <typename U,
    typename std::enable_if<!std::is_void<U>::value &&
                                !detail::is_unbounded_array<U>::value,
        int>::type>
offset_ptr<T> offset_ptr<T>::operator-(offset_ptr::difference_type diff) const {
  GPCL_ASSERT(_real_ptr() != 0);

  offset_ptr ret = *this;
  ret -= diff;
  return ret;
}

template <typename T>
template <typename Dummy,
    typename std::enable_if<std::is_convertible<Dummy *, const char *>::value,
        int>::type>
bool offset_ptr<T>::operator!=(const std::string &str) const {
  GPCL_ASSERT(_real_ptr() != 0);
  return _real_ptr() != str;
}

} // namespace gpcl

#endif // GPCL_IMPL_OFFSET_PTR_HPP
