//
// clone_ptr.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_CLONE_PTR_HPP
#define GPCL_CLONE_PTR_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/utility.hpp>
#include <memory>

namespace gpcl {

template <typename T, typename Void = void>
struct is_cloneable : std::false_type
{
};

template <typename T>
struct is_cloneable<
    T, std::enable_if_t<std::is_constructible<
           std::unique_ptr<T>, decltype(std::declval<T &>().clone())>::value>>
    : std::true_type
{
};

/// clone_ptr is a smart pointer that automatically clones the managed object.
///
/// \requires T shall be *Cloneable*.
///
/// \requires Deleter Deleter must be *FunctionObject* or lvalue reference to a
/// *FunctionObject* or lvalue reference to function, callable with an argument
/// of type `unique_ptr<T, Deleter>::pointer`.
template <typename T, typename Deleter = std::default_delete<T>>
class clone_ptr
{
public:
  static_assert(is_cloneable<T>::value, "T shall be cloneable");

  /// Default constructor.
  constexpr clone_ptr() = default;

  /// Create an empty clone_ptr.
  constexpr clone_ptr(std::nullptr_t) noexcept {}

  /// Move constructor.
  clone_ptr(clone_ptr &&other) noexcept
      : ptr_(detail::exchange(other.ptr_, nullptr))
  {
  }

  /// Copy constructor.
  /// \effects If `bool(other)` is true, the managed object will be cloned.
  clone_ptr(const clone_ptr &other)
      : clone_ptr(other ? other->clone() : nullptr)
  {
  }

  /// Create a clone_ptr from a unique_ptr.
  clone_ptr(std::unique_ptr<T, Deleter> unique_ptr) noexcept
      : ptr_(unique_ptr.release())
  {
  }

  /// Create a clone_ptr from raw pointer and deleter.
  explicit clone_ptr(T *p, Deleter d = Deleter()) noexcept
      : ptr_(p),
        deleter_(std::move(d))
  {
  }

  /// Move assignment.
  clone_ptr &operator=(clone_ptr &&other) noexcept
  {
    swap(other);
    return *this;
  }

  /// Copy assignment.
  /// \effects If `bool(other)` is true, the managed object will be cloned.
  clone_ptr &operator=(const clone_ptr &other)
  {
    if (std::addressof(other) == this)
      return *this;

    reset(other ? other->clone() : nullptr);
    return *this;
  }

  /// Create a clone_ptr from a unique_ptr
  clone_ptr &operator=(std::unique_ptr<T, Deleter> unique_ptr) noexcept
  {
    ptr_ = unique_ptr.release();
    return *this;
  }

  /// Destructor.
  ~clone_ptr() noexcept
  {
    if (ptr_)
      deleter_(ptr_);
  }

  /// Swap the pointers.
  void swap(clone_ptr &other) noexcept
  {
    using std::swap;
    swap(ptr_, other.ptr_);
  }

  /// Swap the pointers.
  friend inline void swap(clone_ptr<T> &x, clone_ptr<T> &y) noexcept
  {
    return x.swap(y);
  }

  /// Determine whether this clone_ptr is empty.
  explicit operator bool() const noexcept { return ptr_ != nullptr; }

  /// Dereference operator.
  /// \requires `bool(*this)` is `true`.
  T &operator*() const
  {
    GPCL_ASSERT(*this);
    return *ptr_;
  }

  /// Member-access operator.
  /// \requires `bool(*this)` is `true`.
  T *operator->() const
  {
    GPCL_ASSERT(*this);
    return ptr_;
  }

  /// Replace the managed pointer.
  void reset(T *ptr = nullptr, Deleter d = Deleter()) noexcept
  {
    if (ptr_)
      deleter_(ptr_);
    ptr_ = ptr;
    deleter_ = std::move(d);
  }

  /// Release the ownership of the managed object.
  T *release() { return detail::exchange(ptr_, nullptr); }

private:
  T *ptr_{};

  Deleter deleter_{};
};

} // namespace gpcl

#endif // GPCL_CLONE_PTR_HPP
