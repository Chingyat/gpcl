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


  /// Segregates a memory block of size sz into as many partition_sz-sized
  /// chunks as possible.
  ///
  /// @param block pointer to the block
  /// @param sz size in bytes
  /// @param partition_sz chunk size
  /// @param end the last chunk's next ptr
  /// @return pointer to the first chunk.
  void *segregate(void *const block, size_type sz, size_type partition_sz,
                  void *end = nullptr)
  {
    GPCL_ASSERT(block);
    GPCL_ASSERT(sz >= partition_sz);

    void *chunk = block;
    void *block_end = reinterpret_cast<char *>(block) + sz - partition_sz;
    while (reinterpret_cast<void *>(reinterpret_cast<char *>(chunk) +
                                    partition_sz) < block_end)
    {
      next_chunk(chunk) = reinterpret_cast<char *>(chunk) + partition_sz;
      chunk = next_chunk(chunk);
    }
    next_chunk(chunk) = end;
    return block;
  }

  /// Segregates a memory block of size sz, and adds it to the free list.
  void add_block(void *const block, size_type sz, size_type partition_sz)
  {
    free_list_ = segregate(block, sz, partition_sz, free_list_);
    GPCL_VERIFY(free_list_);
  }

  void add_ordered_block(void *const block, size_type sz,
                         size_type partition_sz)
  {
    void *&pos = upper_bound(block);
    pos = segregate(block, sz, partition_sz, pos);
    GPCL_VERIFY(free_list_);
  }

  bool empty() const { return free_list_ == nullptr; }

  void *malloc(size_type partition_sz)
  {
    GPCL_ASSERT(!empty());
    auto ret = free_list_;
    free_list_ = next_chunk(free_list_);
    return ret;
  }

  void *malloc_n(size_type n, size_type partition_sz)
  {
    void **p = &free_list_;
    void **start = p;
    size_type m = 0;
    while (*p)
    {
      std::ptrdiff_t diff = reinterpret_cast<char *>(next_chunk(*p)) -
                            reinterpret_cast<char *>(*p);
      if (diff != partition_sz)
      {
        m = 0;
        p = &next_chunk(*p);
        start = p;
      }
      else
      {
        ++m;
        p = &next_chunk(*p);

        if (m == n)
        {
          auto ret = *start;
          *start = *p;
          return ret;
        }
      }
    }

    return nullptr;
  }

  void free(void *const chunk)
  {
    next_chunk(chunk) = free_list_;
    free_list_ = chunk;
  }

  void free_n(void *const chunk, size_type partition_sz, size_type n)
  {
    add_block(chunk, partition_sz * n, partition_sz);
  }

  void ordered_free(void *const chunk)
  {
    void *&pos = upper_bound(chunk);
    next_chunk(chunk) = pos;
    pos = chunk;
  }

  void ordered_free_n(void *const chunk, size_type n, size_type partition_sz)
  {
    add_ordered_block(chunk, partition_sz * n, partition_sz);
  }

private:
  void *&upper_bound(void *const p)
  {
    void **pp = &free_list_;
    while (*pp)
    {
      void *&next = next_chunk(*pp);
      if (next > p)
      {
        return next;
      }
      pp = &next;
    }
    return *pp;
  }

  static void *&next_chunk(void *chunk)
  {
    GPCL_ASSERT(chunk);
    return *reinterpret_cast<void **>(chunk);
  }

  void *free_list_{};
};

} // namespace gpcl

#endif // GPCL_SIMPLE_SEGREGATED_STORAGE_HPP
