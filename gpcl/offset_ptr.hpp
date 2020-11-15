//
// offset_ptr.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OFFSET_PTR_HPP
#define GPCL_OFFSET_PTR_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/type_traits.hpp>
#include "narrow_cast.hpp"
#include <limits>
#include <string>

namespace gpcl {

template <typename T>
class offset_ptr;

template <typename T>
struct is_offset_ptr : std::false_type
{
};

template <typename T>
struct is_offset_ptr<offset_ptr<T>> : std::true_type
{
};

namespace detail {
template <typename T, typename = void>
class offset_ptr_base
{
public:
  typedef T &reference;
};

template <typename T>
class offset_ptr_base<T, typename std::enable_if<std::is_void<T>::value>::type>
{
public:
};

template <typename T>
std::intptr_t as_integer(T *ptr)
{
  return reinterpret_cast<std::intptr_t>(ptr);
}

} // namespace detail

/// A fancy pointer class that stores offset instead of virtual address in it.
template <typename T>
class offset_ptr : public detail::offset_ptr_base<T>
{
private:
  using iterator_category = std::random_access_iterator_tag;

  using value_type = typename std::decay<T>::type;
  typedef T *pointer;
  std::size_t typedef size_type;
  std::ptrdiff_t typedef difference_type;
  std::intptr_t typedef offset_type;

  constexpr static offset_type poison =
      (std::numeric_limits<offset_type>::max)();
  offset_type offset_; // offset in bytes to this

  GPCL_DECL_INLINE constexpr pointer _real_ptr() const;

public:
  /// Constructor
  GPCL_DECL_INLINE constexpr offset_ptr(std::nullptr_t = nullptr)
      : offset_(poison)
  {
  }

  /// Constructor
  offset_ptr(T *ptr);

  /// Copy constructor
  GPCL_DECL_INLINE constexpr offset_ptr(const offset_ptr &other)
      : offset_ptr(other._real_ptr())
  {
  }

  /// Converts to raw pointer
  GPCL_DECL_INLINE constexpr operator pointer() const { return _real_ptr(); }

  /// Copy assignment
  GPCL_DECL_INLINE constexpr offset_ptr &operator=(const offset_ptr &other);

  /// Converts pointer difference to offset
  GPCL_DECL_INLINE constexpr static offset_type
  diff_to_offset(difference_type diff);

  /// Converts offset to pointer difference
  GPCL_DECL_INLINE constexpr static difference_type
  offset_to_diff(offset_type offset);

  /// Dereference
  /// @return reference to the value
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value, int>::type = 0>
  GPCL_DECL_INLINE auto &operator*() const;

  /// Member access operator
  /// @return a raw pointer
  GPCL_DECL_INLINE pointer operator->() const;

  /// Increment the offset_ptr
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE offset_ptr &operator++() &;

  /// Increment the offset_ptr and returns the original value
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  [[nodiscard]] GPCL_DECL_INLINE const offset_ptr operator++(int) &;

  /// Decrement the offset_ptr
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE offset_ptr &operator--() &;

  /// Decrement the offset_ptr and return the original value
  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  [[nodiscard]] GPCL_DECL_INLINE offset_ptr operator--(int) &;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE offset_ptr &operator+=(difference_type diff);

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE offset_ptr &operator-=(difference_type diff);

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE difference_type operator-(const offset_ptr &other) const;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE offset_ptr operator+(difference_type diff) const;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE offset_ptr operator-(difference_type diff) const;

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  friend GPCL_DECL_INLINE offset_ptr operator+(difference_type diff,
                                               const offset_ptr &ptr)
  {
    return ptr + diff;
  }

  template <typename U = T,
            typename std::enable_if<!std::is_void<U>::value &&
                                        !detail::is_unbounded_array<U>::value,
                                    int>::type = 0>
  GPCL_DECL_INLINE U &operator[](difference_type diff) const
  {
    return *(*this + diff);
  }

  GPCL_DECL_INLINE constexpr bool operator==(const offset_ptr &other) const
  {
    return this->_real_ptr() == other._real_ptr();
  }

  GPCL_DECL_INLINE constexpr bool operator!=(const offset_ptr &other) const
  {
    return !(*this == other);
  }

  GPCL_DECL_INLINE constexpr bool operator==(std::nullptr_t) const
  {
    return this->_real_ptr() == nullptr;
  }

  GPCL_DECL_INLINE constexpr bool operator!=(std::nullptr_t) const
  {
    return this->_real_ptr() != nullptr;
  }

  GPCL_DECL_INLINE constexpr bool operator==(pointer ptr) const
  {
    return this->_real_ptr() == ptr;
  }

  GPCL_DECL_INLINE constexpr bool operator!=(pointer ptr) const
  {
    return this->_real_ptr() == ptr;
  }

  template <
      typename Dummy = T,
      typename std::enable_if<std::is_convertible<Dummy *, const char *>::value,
                              int>::type = 0>
  GPCL_DECL_INLINE constexpr bool operator==(const std::string &str) const
  {
    GPCL_ASSERT(_real_ptr() != 0);
    return _real_ptr() == str;
  }

  friend GPCL_DECL_INLINE constexpr bool operator<(const offset_ptr &x,
                                                   const offset_ptr &y)
  {
    return std::addressof(*x) < std::addressof(*y);
  }

  friend GPCL_DECL_INLINE constexpr bool operator>(const offset_ptr &x,
                                                   const offset_ptr &y)
  {
    return y < x;
  }

  friend GPCL_DECL_INLINE constexpr bool operator<=(const offset_ptr &x,
                                                    const offset_ptr &y)
  {
    return !(x > y);
  }

  friend GPCL_DECL_INLINE constexpr bool operator>=(const offset_ptr &x,
                                                    const offset_ptr &y)
  {
    return !(x < y);
  }

  GPCL_DECL_INLINE constexpr explicit operator bool() const
  {
    return _real_ptr() != nullptr;
  }

  template <
      typename Dummy = T,
      typename std::enable_if<std::is_convertible<Dummy *, const char *>::value,
                              int>::type = 0>
  bool operator!=(const std::string &str) const;
};

} // namespace gpcl

#ifndef GPCL_STANDARDESE
#  include <gpcl/impl/offset_ptr.hpp>
#endif

#endif
