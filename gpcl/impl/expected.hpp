//
// expected.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_IMPL_EXPECTED_HPP
#define GPCL_IMPL_EXPECTED_HPP

#include <gpcl/expected.hpp>
#include <gpcl/optional.hpp>

namespace gpcl {

template <typename T, typename E>
optional<T> expected<T, E>::as_optional() const & {
  if (*this)
    return optional<T>(**this);
  return nullopt;
}

template <typename T, typename E> optional<T> expected<T, E>::as_optional() && {
  if (*this)
    return optional<T>(*detail::move(*this));
  return nullopt;
}

template <typename T, typename E>
optional<E> expected<T, E>::as_optional_error() const & {
  if (!*this)
    return optional<T>(this->error());
  return nullopt;
}

template <typename T, typename E>
optional<E> expected<T, E>::as_optional_error() && {
  if (!*this)
    return optional<T>(detail::move(*this).error());
  return nullopt;
}
template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<!detail::is_convertible_v<const G &, E> &&
                         detail::is_void_v<T> && detail::is_void_v<U>,
        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs) {
  this->ok_ = bool(rhs);
  if (!this->ok_) {
    new (&this->val_) unexpected<E>(rhs.error());
  }
}

template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<!detail::is_convertible_v<G &&, E> &&
                         detail::is_void_v<T> && detail::is_void_v<U>,
        int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs) {
  this->ok_ = bool(rhs);
  if (!this->ok_) {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}

template <typename T, typename E>
template <typename U, std::enable_if_t<!detail::is_void_v<U>, int>>
expected<T, E> &expected<T, E>::operator=(U &&v) {

  static_assert(
      !detail::is_same_v<expected, std::decay_t<U>> &&
      !(std::is_scalar<T>::value && std::is_same<T, std::decay_t<U>>::value) &&
      detail::is_constructible_v<T, U> && detail::is_assignable_v<T &, U> &&
      detail::is_nothrow_move_constructible_v<E>);

  if (*this) {
    this->val_ = detail::forward<U>(v);
  } else {
    if constexpr (detail::is_nothrow_constructible_v<T, U>) {
      this->err_.unexpected<E>::~unexpected();
      new (&this->val_) T(detail::forward<U>(v));
      this->ok_ = true;
    } else {
      unexpected<E> tmp(detail::move(this->error()));
      this->err_.unexpected<E>::~unexpected();
      try {
        new (&this->val_) T(detail::forward<U>(v));
      } catch (...) {
        new (&this->err_) unexpected<E>(detail::move(tmp));
      }
      this->ok_ = true;
    }
  }
  return *this;
}

template <typename T, typename E>
template <typename G,
    std::enable_if_t<detail::is_void_v<T> &&
                         detail::is_nothrow_copy_constructible_v<G> &&
                         detail::is_copy_assignable_v<G>,
        int>>
expected<T, E> &expected<T, E>::operator=(const unexpected<G> &e) {
  if (*this) {
    new (&this->err_) unexpected<E>(e.value());
    this->ok_ = false;
  } else {
    this->err_ = e;
  }
  return *this;
}

template <typename T, typename E>
template <typename G,
    std::enable_if_t<detail::is_void_v<T> &&
                         detail::is_nothrow_move_constructible_v<G> &&
                         detail::is_move_assignable_v<G>,
        int>>
expected<T, E> &expected<T, E>::operator=(unexpected<G> &&e) {
  if (*this) {
    new (&this->err_) unexpected<E>(detail::move(e.value()));
    this->ok_ = false;
  } else {
    this->err_ = detail::move(e);
  }
  return *this;
}

template <typename T, typename E>
template <typename G,
    std::enable_if_t<!detail::is_void_v<T> &&
                         detail::is_nothrow_move_constructible_v<G> &&
                         detail::is_move_assignable_v<G>,
        int>>
expected<T, E> &expected<T, E>::operator=(unexpected<G> &&e) {
  if (*this) {
    this->val_.T::~T();
    new (&this->err_) unexpected<E>(detail::move(e.value()));
    this->ok_ = false;
  } else {
    this->err_ = detail::move(e);
  }
  return *this;
}

template <typename T, typename E>
template <typename G,
    std::enable_if_t<!detail::is_void_v<T> &&
                         detail::is_nothrow_copy_constructible_v<G> &&
                         detail::is_copy_assignable_v<G>,
        int>>
expected<T, E> &expected<T, E>::operator=(const unexpected<G> &e) {
  if (*this) {
    this->val_.T::~T();
    new (&this->err_) unexpected<E>(e.value());
    this->ok_ = false;
  } else {
    this->err_ = e;
  }
  return *this;
}

template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<detail::is_convertible_v<G &&, E> &&
                         detail::is_void_v<T> && detail::is_void_v<U>,
        int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs) {
  this->ok_ = bool(rhs);
  if (!this->ok_) {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}

template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<(!detail::is_convertible_v<U &&, T> ||
                         !detail::is_convertible_v<G &&, E>)&&detail::
                         expected_convert_constructible_v<T, E, U, G>,
        int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs) {
  this->ok_ = bool(rhs);
  if (this->ok_) {
    new (&this->val_) T(detail::move(*rhs));
  } else {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}
template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<(detail::is_convertible_v<U &&, T> &&
                         detail::is_convertible_v<G &&, E>)&&detail::
                         expected_convert_constructible_v<T, E, U, G>,
        int>>
constexpr expected<T, E>::expected(expected<U, G> &&rhs) {
  this->ok_ = bool(rhs);
  if (this->ok_) {
    new (&this->val_) T(detail::move(*rhs));
  } else {
    new (&this->err_) unexpected<E>(detail::move(rhs).error());
  }
}

template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<detail::is_convertible_v<const G &, E> &&
                         detail::is_void_v<T> && detail::is_void_v<U>,
        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs) {
  this->ok_ = bool(rhs);
  if (!this->ok_) {
    new (&this->val_) unexpected<E>(rhs.error());
  }
}

template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<(!detail::is_convertible_v<const U &, T> ||
                         !detail::is_convertible_v<const G &, E>)&&detail::
                         expected_convert_constructible_v<T, E, U, G>,
        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs) {
  this->ok_ = bool(rhs);
  if (this->ok_) {
    new (&this->val_) T(*rhs);
  } else {
    new (&this->err_) unexpected<E>(rhs.error());
  }
}

template <typename T, typename E>
template <typename U, typename G,
    std::enable_if_t<(detail::is_convertible_v<const U &, T> &&
                         detail::is_convertible_v<const G &, E>)&&detail::
                         expected_convert_constructible_v<T, E, U, G>,
        int>>
constexpr expected<T, E>::expected(const expected<U, G> &rhs) {
  this->ok_ = bool(rhs);
  if (this->ok_) {
    new (&this->val_) T(*rhs);
  } else {
    new (&this->err_) unexpected<E>(rhs.error());
  }
}

} // namespace gpcl

#endif