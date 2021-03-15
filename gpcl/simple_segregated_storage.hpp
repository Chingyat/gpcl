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
#include <atomic>
#include <cassert>
#include <memory>
#include <numeric>

namespace gpcl {

/// Simple segregated storage algorithm implementation.
template <typename SizeType = std::size_t>
class simple_segregated_storage : noncopyable
{
public:
  using size_type = SizeType;

  /// \effects Interleaves a free list through the memory block specified of
  /// `block` of size `sz` bytes, partitioning it into as many
  /// partition_sz-sized chunks as possible. The last chunk is set to point to
  /// `end`. \preconditions partition_sz >= sizeof(void *), partition_sz ==
  /// sizeof(void *) * i for some i, sz >= partition_sz, block is properly
  /// aligned for an array of objects of size partition_sz, and block is
  /// properly aligned for an array of void *. \param block pointer to the block
  /// \param sz size in bytes
  /// \param partition_sz chunk size
  /// \param end the last chunk's next ptr
  /// \returns pointer to the first chunk (this is always equal to block).
  /// \complexity O(sz).
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

  /// \effects Segregates a memory block of size sz, and adds it to the free
  /// list.
  ///
  /// \param block pointer to a block of memory.
  /// \param sz Size in bytes
  /// of the memory block.
  /// \param partition_sz Size per chunk.
  /// \postconditions !this->empty().
  ///
  /// \notes If `this` was empty before calling this function,
  /// then it is **ordered** after this call.
  ///
  /// \complexity O(sz).
  void add_block(void *const block, size_type sz, size_type partition_sz)
  {
    free_list_ = segregate(block, sz, partition_sz, free_list_);
    GPCL_VERIFY(free_list_);
  }

  /// \effects Segregates a memory block of size sz, and adds it to the free
  /// list.
  ///
  /// \param block pointer to a block of memory.
  /// \param sz Size in bytes
  /// of the memory block.
  /// \param partition_sz Size per chunk.
  ///
  /// \postconditions
  /// !this->empty().
  ///
  /// \notes If `this` is ordered before calling this function,
  /// it will remain ordered after calling this function.
  ///
  /// \complexity O(sz).
  void add_ordered_block(void *const block, size_type sz,
                         size_type partition_sz)
  {
    void *&pos = upper_bound(block);
    pos = segregate(block, sz, partition_sz, pos);
    GPCL_VERIFY(free_list_);
  }

  /// \returns true if `this` is empty.
  /// \notes If `this` is ordered before calling this function, it will remain
  /// ordered after calling this function.
  bool empty() const { return free_list_ == nullptr; }

  /// \effects Remove the first chunk from the free list.
  /// \preconditions !this->empty().
  /// \returns Pointer to the removed chunk.
  /// \notes If `this` is ordered before calling this function, it will remain
  /// ordered after calling this function.
  ///
  /// \complexity O(1).
  void *malloc(size_type partition_sz)
  {
    GPCL_ASSERT(!empty());
    auto ret = free_list_;
    free_list_ = next_chunk(free_list_);
    return ret;
  }

  /// \effects Attempt to find a sequence a n partition_sz-sized chunks. If
  /// found, remove them from the free list.
  ///
  /// \returns Pointer to the first removed chunk.
  ///
  /// \notes It is strongly recommended (but not required) that
  /// the free list be ordered, as this algorithm will fail to find a contiguous
  /// sequence unless it is contiguous in the free list as well.
  /// If `this` is ordered before calling this function, it will remain
  /// ordered after calling this function.
  /// \complexity O(N) where N is the size of the free list.
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

  /// \effects Put chunk back to the free list.
  ///
  /// \preconditions chunk was previously returned from a call to
  /// this->malloc().
  ///
  /// \complexity O(1).
  void free(void *const chunk)
  {
    next_chunk(chunk) = free_list_;
    free_list_ = chunk;
  }

  /// \effects Put an array of n chunks back to the free list.
  ///
  /// \preconditions the array of chunks was previously returned from a call to
  /// this->malloc_n().
  ///
  /// \complexity O(1).
  void free_n(void *const chunk, size_type partition_sz, size_type n)
  {
    add_block(chunk, partition_sz * n, partition_sz);
  }

  /// \effects Put chunk back to the free list.
  ///
  /// \preconditions chunk was previously returned from a call to
  /// this->malloc().
  ///
  /// \notes If `this` is ordered before calling this function,
  /// it will remain ordered after calling this function.
  ///
  /// \complexity O(1).
  void ordered_free(void *const chunk)
  {
    void *&pos = upper_bound(chunk);
    next_chunk(chunk) = pos;
    pos = chunk;
  }

  /// \effects Put an array of n chunks back to the free list.
  ///
  /// \preconditions the array of chunks was previously returned from a call to
  /// this->malloc_n().
  ///
  /// \notes If `this` is ordered before calling this function,
  /// it will remain ordered after calling this function.
  ///
  /// \complexity O(1).
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

  std::atomic<void *> free_list_{};
};

} // namespace gpcl

#endif // GPCL_SIMPLE_SEGREGATED_STORAGE_HPP
