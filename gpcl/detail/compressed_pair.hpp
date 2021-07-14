//
// compressed_pair.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_COMPRESSED_PAIR_HPP
#define GPCL_DETAIL_COMPRESSED_PAIR_HPP

#include <gpcl/detail/config.hpp>
#include <tuple>
#include <type_traits>

namespace gpcl {
namespace detail {

template <typename T, typename Tag, bool IsClass = std::is_class<T>::value>
class compressed_pair_base;

template <typename T, typename Tag>
class compressed_pair_base<T, Tag, false>
{
  T data_;

public:
  using type = T;

  template <typename... Args>
  constexpr compressed_pair_base(Args &&...args)
      : data_(std::forward<Args>(args)...)
  {
  }

  T &get() & { return data_; }
  T &&get() && { return std::move(data_); }

  T const &get() const & { return data_; }
  T const &&get() const && { return data_; }
};

template <typename T, typename Tag>
class compressed_pair_base<T, Tag, true> : private T
{
public:
  using type = T;

  template <typename... Args>
  constexpr compressed_pair_base(Args &&...args)
      : T(std::forward<Args>(args)...)
  {
  }

  T &get() & { return *this; }
  T &&get() && { return std::move(*this); }

  T const &get() const & { return *this; }
  T const &&get() const && { return *this; }
};

template <typename T1, typename T2>
class compressed_pair : private compressed_pair_base<T1, class tag1>,
                        private compressed_pair_base<T2, class tag2>
{
  using base_type_1 = compressed_pair_base<T1, tag1>;
  using base_type_2 = compressed_pair_base<T2, tag2>;

  template <typename Tuple1, typename Tuple2, std::size_t... Is,
            std::size_t... Js>
  constexpr compressed_pair(Tuple1 &&tp1, Tuple2 &&tp2,
                            std::index_sequence<Is...>,
                            std::index_sequence<Js...>)
      : base_type_1(std::get<Is>(std::forward<Tuple1>(tp1))...),
        base_type_2(std::get<Js>(std::forward<Tuple2>(tp2))...)
  {
  }

public:
  using first_type = T1;
  using second_type = T2;

  enum piecewise_construct_t
  {
    piecewise_construct
  };

  template <typename U1, typename U2>
  constexpr compressed_pair(U1 &&x, U2 &&y)
      : base_type_1(std::forward<U1>(x)),
        base_type_2(std::forward<U2>(y))
  {
  }

  template <typename Tuple1, typename Tuple2>
  constexpr compressed_pair(piecewise_construct_t, Tuple1 &&tp1, Tuple2 &&tp2)
      : compressed_pair(std::forward<Tuple1>(tp1), std::forward<Tuple2>(tp2),
                        std::make_index_sequence<
                            std::tuple_size<std::decay_t<Tuple1>>::value>(),
                        std::make_index_sequence<
                            std::tuple_size<std::decay_t<Tuple2>>::value>())
  {
  }

  T1 &first() & { return base_type_1::get(); }

  T1 &&first() && { return std::move(*this).base_type_1::get(); }

  T1 const &first() const & { return base_type_1::get(); }

  T1 const &&first() const && { return std::move(*this).base_type_1::get(); }

  T2 &second() & { return base_type_2::get(); }

  T2 &&second() && { return std::move(*this).base_type_2::get(); }

  T2 const &second() const & { return base_type_2::get(); }

  T2 const &&second() const && { return std::move(*this).base_type_2::get(); }
};

} // namespace detail
} // namespace gpcl

#endif // GPCL_DETAIL_COMPRESSED_PAIR_HPP
