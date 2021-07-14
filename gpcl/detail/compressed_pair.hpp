#ifndef GPCL_DETAIL_COMPRESSED_PAIR_HPP
#define GPCL_DETAIL_COMPRESSED_PAIR_HPP

#include <gpcl/detail/config.hpp>
#include <type_traits>

namespace gpcl {
namespace detail {

template <typename T, typename Tag, bool IsClass = std::is_class<T>::value>
class compressed_pair_base;

template <typename T, typename Tag>
class compressed_pair_base<T, Tag, false> {
    T data_;
public:
    using type = T;

    template <typename ...Args>
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
class compressed_pair_base<T, Tag, true> : private T {
public:
    using type = T;

    template <typename ...Args>
    constexpr compressed_pair_base(Args &&...args)
        : T(std::forward<Args>(args)...)
    {
    }

    T &get() & { return *this; }
    T &&get() && { return std::move(*this); }

    T const &get() const & { return *this; }
    T const &&get() const && { return *this; }
};

class compressed_tag_1;
class compressed_tag_2;


template <typename T1, typename T2>
class compressed_pair : 
    private compressed_pair_base<T1, compressed_tag_1>, 
    private compressed_pair_base<T2, compressed_tag_2>  {

    using base_type_1 = compressed_pair_base<T1, compressed_tag_1>;
    using base_type_2 = compressed_pair_base<T2, compressed_tag_2>;

public:
    using first_type = T1;
    using second_type = T2;

    template <typename U1, typename U2>
    compressed_pair(U1 &&x, U2 &&y)
        : base_type_1(std::forward<U1>(x)),
          base_type_2(std::forward<U2>(y))
    {
    }

    T1 &first() &
    {
        return base_type_1::get();
    }

    T1 && first() &&
    {
        return std::move(*this).base_type_1::get();
    }

    T1 const &first() const &
    {
        return base_type_1::get();
    }

    T1 const && first() const &&
    {
        return std::move(*this).base_type_1::get();
    }
  
    T2 &second() &
    {
        return base_type_2::get();
    }

    T2 && second() &&
    {
        return std::move(*this).base_type_2::get();
    }

    T2 const &second() const &
    {
        return base_type_2::get();
    }

    T2 const && second() const &&
    {
        return std::move(*this).base_type_2::get();
    }
    
};

}
}

#endif // GPCL_DETAIL_COMPRESSED_PAIR_HPP
