//
// optional.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OPTIONAL_HPP
#define GPCL_OPTIONAL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/optional.hpp>
#include <gpcl/error.hpp>
#include <gpcl/optional_fwd.hpp>

namespace gpcl {

class bad_optional_access : public std::runtime_error {
public:
  bad_optional_access() : std::runtime_error("bad_optional_access") {}
};

/// Represents an optional value: either some value T or null.
template <typename T>
class optional : public detail::optional_move_assign_base<T> {
  using base_type = detail::optional_move_assign_base<T>;

public:
  using value_type = T;

  /// @name constructors
  /// @{
  GPCL_DECL_INLINE constexpr optional() noexcept = default;
  GPCL_DECL_INLINE constexpr optional(nullopt_t) noexcept {}
  GPCL_DECL_INLINE constexpr optional(const optional &other) = default;
  GPCL_DECL_INLINE constexpr optional(optional &&) noexcept(
      std::is_nothrow_move_constructible<T>()) = default;
  template <typename... Args,
      typename std::enable_if<std::is_constructible<T, Args...>::value,
          int>::type = 0>
  GPCL_DECL_INLINE constexpr explicit optional(in_place_t, Args &&... args)
      : base_type(in_place, detail::forward<Args>(args)...) {}

  template <typename U, class... Args>
  GPCL_DECL_INLINE constexpr explicit optional(
      in_place_t, std::initializer_list<U> ilist, Args &&... args)
      : base_type(in_place, ilist, detail::forward<Args>(args)...) {}

  template <typename U = T,
      typename std::enable_if<std::is_constructible<T, U &&>::value &&
                                  !std::is_convertible<U &&, T>::value,
          int>::type = 0>
  GPCL_DECL_INLINE explicit constexpr optional(U &&x)
      : base_type(in_place, detail::forward<U>(x)) {}

  template <typename U = T,
      typename std::enable_if<std::is_constructible<T, U &&>::value &&
                                  std::is_convertible<U &&, T>::value,
          int>::type = 0>
  GPCL_DECL_INLINE constexpr optional(U &&x)
      : base_type(in_place, detail::forward<U>(x)) {}

  template <typename U,
      typename std::enable_if<
          std::is_constructible<T, const U &>::value &&
              detail::optional_convert_constructible<U, T>::value &&
              !std::is_convertible<const U &, T>::value,
          int>::type = 0>
  GPCL_DECL_INLINE explicit optional(const optional<U> &other) {
    this->has_val_ = other.has_val_;
    if (this->has_val_) {
      new (&this->val_) T(other.val_);
    }
  }

  template <typename U,
      typename std::enable_if<
          std::is_constructible<T, const U &>::value &&
              detail::optional_convert_constructible<U, T>::value &&
              std::is_convertible<const U &, T>::value,
          int>::type = 0>
  GPCL_DECL_INLINE optional(const optional<U> &other) {
    this->has_val_ = other.has_val_;
    if (this->has_val_) {
      new (&this->val_) T(other.val_);
    }
  }

  template <typename U,
      typename std::enable_if<
          std::is_constructible<T, U &&>::value &&
              detail::optional_convert_constructible<U, T>::value &&
              !std::is_convertible<U &&, T>::value,
          int>::type = 0>
  GPCL_DECL_INLINE explicit optional(optional<U> &&other) {
    this->has_val_ = detail::exchange(other.has_val_, false);
    if (this->has_val_) {
      new (&this->val_) T(other.val_);
    }
  }

  template <typename U,
      typename std::enable_if<
          std::is_constructible<T, U &&>::value &&
              detail::optional_convert_constructible<U, T>::value &&
              std::is_convertible<U &&, T>::value,
          int>::type = 0>
  GPCL_DECL_INLINE optional(optional<U> &&other) {
    this->has_val_ = detail::exchange(other.has_val_, false);
    if (this->has_val_) {
      new (&this->val_) T(other.val_);
    }
  }

  ///@}

  /// destructor
  GPCL_DECL_INLINE ~optional() = default;

  /// @name assignment
  /// @{
  GPCL_DECL_INLINE optional &operator=(nullopt_t) noexcept {
    if (detail::exchange(this->has_val_, false)) {
      this->val_.T::~T();
    }
  }

  GPCL_DECL_INLINE constexpr optional &operator=(
      const optional &other) = default;
  GPCL_DECL_INLINE constexpr optional &operator=(optional &&other) noexcept(
      std::is_nothrow_move_constructible<T>::value
          &&std::is_nothrow_move_assignable<T>::value) = default;

  template <typename U = T,
      typename std::enable_if<
          std::is_constructible<T, U>::value &&
              std::is_assignable<T &, U>::value &&
              (!std::is_scalar<T>::value ||
                  !std::is_same<typename std::decay<U>::type, T>::value),
          int>::type = 0>
  GPCL_DECL_INLINE optional &operator=(U &&u) {
    if (this->has_val_) {
      this->val_ = detail::forward<U>(u);
    } else {
      new (&this->val_) T(detail::forward<U>(u));
      this->has_val_ = true;
    }
    return *this;
  }

  template <typename U, typename std::enable_if<
                            detail::optional_convert_assignable<U, T>::value &&
                                std::is_constructible<T, const U &>::value &&
                                std::is_assignable<T &, const U &>::value,
                            int>::type = 0>
  GPCL_DECL_INLINE optional &operator=(const optional<U> &other) {
    if (this->has_val_ && other.has_val_) {
      this->val_ = other.val_;
    } else if (this->has_val_ && !other.has_val_) {
      this->val_.T::~T();
      this->has_val_ = false;
    } else if (!this->has_val_ && other.has_val_) {
      this->has_val_ = true;
      new (&this->val_) T(other.val_);
    } else {
      // do nothing
    }
    return *this;
  }

  template <typename U, typename std::enable_if<
                            detail::optional_convert_assignable<U, T>::value &&
                                std::is_constructible<T, U &&>::value &&
                                std::is_assignable<T &, U>::value,
                            int>::type = 0>
  GPCL_DECL_INLINE optional &operator=(optional<U> &&other) {
    if (this->has_val_ && other.has_val_) {
      this->val_ = std::move(other.val_);
      other.has_val_ = false;
    } else if (this->has_val_ && !other.has_val_) {
      this->val_.T::~T();
      this->has_val_ = false;
    } else if (!this->has_val_ && other.has_val_) {
      this->has_val_ = true;
      new (&this->val_) T(std::move(other.val_));
      other.has_val_ = false;
    } else {
      // do nothing
    }
    return *this;
  }

  template <typename... Args> T &emplace(Args &&... args) {
    if (this->has_val_ && !std::is_trivially_destructible<T>()) {
      this->val_.T::~T();
      this->has_val_ = false;
    }
    new (&this->val_) T(detail::forward<Args>(args)...);
    this->has_val_ = true;
    return this->val_;
  }

  template <typename U, class... Args>
  T &emplace(std::initializer_list<U> ilist, Args &&... args) {
    if (this->has_val_ && !std::is_trivially_destructible<T>()) {
      this->val_.T::~T();
      this->has_val_ = false;
    }
    new (&this->val_) T(ilist, detail::forward<Args>(args)...);
    this->has_val_ = true;
    return this->val_;
  }

  /// @}

  // swap
  void swap(optional &other) noexcept(std::is_nothrow_swappable<T>::value
          &&std::is_nothrow_move_constructible<T>::value) {
    if (this->has_val_ && other.has_val_) {
      using std::swap;
      swap(this->val_, other.val_);
    } else if (this->has_val_ && !other.has_val_) {
      other = detail::move(*this);
      this->val_.T::~T();
      this->has_val_ = false;
      other.has_val_ = true;
    } else if (!this->has_val_ && other.has_val_) {
      *this = detail::move(other);
      other.val_.T::~T();
      other.has_val_ = false;
      this->has_val_ = true;
    }
  }

  /// @name observers
  /// @{
  constexpr const T *operator->() const {
    assert(this->has_val_);
    return &this->val_;
  }

  constexpr T *operator->() {
    assert(this->has_val_);
    return &this->val_;
  }

  constexpr const T &operator*() const & {
    assert(this->has_val_);
    return this->val_;
  }

  constexpr T &operator*() & {
    assert(this->has_val_);
    return this->val_;
  }

  constexpr T &&operator*() && {
    assert(this->has_val_);
    return detail::move(this->val_);
  }

  constexpr const T &&operator*() const && {
    assert(this->has_val_);
    return detail::move(this->val_);
  }

  constexpr explicit operator bool() const noexcept { return this->has_val_; }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return this->has_val_;
  }

  constexpr const T &value() const & {
    if (has_value()) {
      return **this;
    }

    GPCL_THROW(bad_optional_access());
  }

  constexpr T &value() & {
    if (has_value()) {
      return **this;
    }

    GPCL_THROW(bad_optional_access());
  }

  constexpr T &&value() && {
    if (has_value()) {
      return detail::move(**this);
    }

    GPCL_THROW(bad_optional_access());
  }
  constexpr const T &&value() const && {
    if (has_value()) {
      return detail::move(**this);
    }

    GPCL_THROW(bad_optional_access());
  }

  template <typename U> constexpr T value_or(U &&default_value) const & {
    return bool(*this) ? **this
                       : static_cast<T>(detail::forward<U>(default_value));
  }

  template <typename U> constexpr T value_or(U &&default_value) && {
    return bool(*this) ? std::move(**this)
                       : static_cast<T>(detail::forward<U>(default_value));
  }

  /// @}

  /// @name modifiers
  /// @{
  void reset() noexcept { *this = nullopt; }
  /// @}
};

#if __cplusplus >= 201703
template <typename T> optional(T) -> optional<T>;
#endif

template <typename T>
constexpr optional<typename std::decay<T>::type> make_optional(T &&x) {
  return optional<typename std::decay<T>::type>(detail::forward<T>(x));
}
template <typename T, class... Args>
constexpr optional<T> make_optional(Args &&... args) {
  return optional<T>(in_place, detail::forward<Args>(args)...);
}
template <typename T, class U, class... Args>
constexpr optional<T> make_optional(
    std::initializer_list<U> il, Args &&... args) {
  return optional<T>(in_place, il, detail::forward<Args>(args)...);
}

} // namespace gpcl

#endif // GPCL_OPTIONAL_HPP
