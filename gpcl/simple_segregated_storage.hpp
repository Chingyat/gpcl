//
// simple_segregated_storage.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_SIMPLE_SEGREGATED_STORAGE_HPP
#define GPCL_SIMPLE_SEGREGATED_STORAGE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <cassert>
#include <memory>
#include <numeric>

namespace gpcl {

template <typename SizeType = std::size_t>
class simple_segregated_storage : noncopyable
{
public:
  using size_type = SizeType;

  simple_segregated_storage() : free_list_() {}

  ~simple_segregated_storage() = default;

  static void *segregate(void *block, size_type sz, size_type partition_sz,
                         void *end = nullptr)
  {
    assert(partition_sz >= sizeof(void *));
    assert(partition_sz % sizeof(void *) == 0);
    assert(sz >= partition_sz);

    size_type n = sz / partition_sz;
    for (size_type i = 0; i != n - 1; ++i)
    {
      *reinterpret_cast<void **>(reinterpret_cast<char *>(block) +
                                 partition_sz * i) =
          reinterpret_cast<char *>(block) + partition_sz * (i + 1);
    }
    *reinterpret_cast<void **>(reinterpret_cast<char *>(block) +
                               partition_sz * (n - 1)) = end;

    return block;
  }

  void add_block(void *block, size_type sz, size_type partition_sz)
  {
    free_list_ = segregate(block, sz, partition_sz, free_list_);
  }

  void add_ordered_block(void *const block, size_type sz,
                         size_type partition_sz)
  {
    void **lower = lower_bound(block);

    *lower =
        segregate(block, sz, partition_sz, (*lower));
  }

  [[nodiscard]] bool empty() const { return free_list_ == nullptr; }

  void *malloc()
  {
    assert(!empty());
    void *ret = free_list_;
    free_list_ = *reinterpret_cast<void **>(free_list_);
    return ret;
  }

  void free(void *const chunk)
  {
    assert(chunk != nullptr);
    *reinterpret_cast<void **>(chunk) = free_list_;
    free_list_ = chunk;
  }

  void ordered_free(void *const chunk)
  {
    void **lower = lower_bound(chunk);
    *reinterpret_cast<void **>(chunk) = (*lower);
    *(lower) = chunk;
  }

  void *malloc_n(size_type n, size_type partition_sz)
  {
    size_type m = 0;
    void **p = &free_list_;
    void **s = p;
    while (*p)
    {
      ++m;

      if (m == n)
      {
        auto ret = *s;
        *s = *reinterpret_cast<void **>(*p);
        return ret;
      }

      if (*reinterpret_cast<char **>(*p) - reinterpret_cast<char *>(*p) !=
          partition_sz)
      {
        m = 0;
        s = p;
      }

      p = &*reinterpret_cast<void **>(*p);
    }
    return nullptr;
  }

  void free_n(void *chunks, size_type n, size_type partition_sz)
  {
    add_block(chunks, n * partition_sz, partition_sz);
  }

  void ordered_free_n(void *chunks, size_type n, size_type partition_sz)
  {
    add_ordered_block(chunks, n * partition_sz, partition_sz);
  }

private:
  // Returns the pointer to the last chunk that greater-equals p
  void **lower_bound(void *p)
  {
    if (free_list_ == nullptr)
      return &free_list_;

    void **p1 = &free_list_;
    while (*p1 && *reinterpret_cast<void **>(*p1) < p)
    {
      p1 = &*reinterpret_cast<void **>(*p1);
    }

    return p1;
  }

  void *free_list_;
};

} // namespace gpcl

#endif // GPCL_SIMPLE_SEGREGATED_STORAGE_HPP
