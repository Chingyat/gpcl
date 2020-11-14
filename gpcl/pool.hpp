#ifndef GPCL_POOL_HPP
#define GPCL_POOL_HPP

#include <gpcl/default_malloc_free_user_allocator.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/simple_segregated_storage.hpp>
#include <gpcl/unique_lock.hpp>

namespace gpcl {

template <typename T, typename = void>
struct is_user_allocator : std::false_type
{
};

template <typename T>
struct is_user_allocator<
    T, std::void_t<decltype(T::malloc(0)), decltype(T::free((void *)0)),
                   typename T::size_type, typename T::difference_type>>
    : std::true_type
{
};

template <typename UA = default_malloc_free_user_allocator,
          typename MutexType = gpcl::mutex>
class pool
{
  static_assert(is_user_allocator<UA>::value, "user allocator");

public:
  using user_allocator_type = UA;
  using mutex_type = MutexType;
  using size_type = typename user_allocator_type::size_type;
  using difference_type = typename user_allocator_type::difference_type;

  /// Constructor.
  explicit pool(size_type requested_size)
      : requested_size_(requested_size),
        chunk_size_(std::lcm(requested_size_,
                             std::lcm(sizeof(void *), sizeof(size_type)))),
        block_list_(),
        next_size_(chunk_size_)
  {
    assert(requested_size != 0);
  }

  /// Destructor.
  ///
  /// During destruction, any malloc'd but not free'd memory will be recycled.
  ~pool() noexcept
  {
    block_info block = block_list_;
    while (block.ptr)
    {
      auto next = *reinterpret_cast<block_info *>(
          block.ptr + block_info_offset(block.size));
      user_allocator_type::free(block.ptr);
      block = next;
    }
  }

  /// Determines the requested size.
  size_type requested_size() const noexcept { return requested_size_; }

  /// Allocates memory for an object of size requested_size.
  [[nodiscard]] void *malloc()
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);

    if (storage_.empty())
    {
      if (request_new_block(chunk_size_))
        return storage_.malloc();
      else
        return nullptr;
    }

    return storage_.malloc();
  }

  /// Allocates memory for an array of n objects of size requested_size.
  [[nodiscard]] void *malloc_n(size_type n)
  {
    assert(n != 0);
    auto count = chunk_count(n);
    gpcl::unique_lock<mutex_type> lock(mutex_);
    if (auto ret = storage_.malloc_n(count, chunk_size_))
      return ret;
    request_new_block(count * chunk_size_);

    return storage_.malloc_n(count, chunk_size_);
  }

  /// Frees memory for an object.
  void free(void *ptr)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.free(ptr);
  }

  /// Frees memory for an array of n objects.
  void free_n(void *ptr, size_type n)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.free_n(ptr, chunk_count(n), chunk_size_);
  }

  /// Frees memory for an object.
  void ordered_free(void *ptr)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.ordered_free(ptr);
  }

  /// Frees memory for an array of objects.
  void ordered_free_n(void *ptr, size_type n)
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);
    storage_.ordered_free_n(ptr, chunk_count(n), chunk_size_);
  }

  // not thread-safe
  size_type get_next_size() const { return next_size_; }

  // not thread-safe
  void set_next_size(size_type next_size)
  {
    GPCL_ASSERT(next_size != 0);
    next_size_ = next_size;
  }

private:
  struct block_info
  {
    // Pointer to the start of the next block.
    char *ptr;

    // Size of the next block.
    size_type size;
  }
#ifdef __GNUC__
  __attribute__((may_alias))
#endif
  ;

  // Requests a block with at least min_size bytes for chunks.
  bool request_new_block(size_type min_size)
  {
    auto sz = next_size_ * chunk_size_;
    while (sz < min_size + sizeof(block_info) * 2)
      sz *= 2;

    next_size_ *= 2;

    char *block = user_allocator_type::malloc(sz);
    if (block == nullptr)
      return false;

    block_list_ = add_block(block, sz, block_list_);
    GPCL_VERIFY_FALSE(storage_.empty());
    return true;
  }

  block_info add_block(char *ptr, size_type sz, block_info end)
  {
    auto chunks_size = block_info_offset(sz);
    storage_.add_block(ptr, chunks_size, chunk_size_);
    *reinterpret_cast<block_info *>(ptr + chunks_size) = end;
    return {ptr, sz};
  }

  difference_type block_info_offset(size_type block_size)
  {
    return (block_size - sizeof(block_info)) / alignof(block_info) *
           alignof(block_info);
  }

  size_type chunk_count(size_type n)
  {
    size_type array_size = n * requested_size_;
    return (array_size + chunk_size_ - 1) / chunk_size_;
  }

  mutable mutex_type mutex_;
  const size_type requested_size_;
  const size_type chunk_size_;
  size_type next_size_;
  block_info block_list_;
  simple_segregated_storage<size_type> storage_;
};

} // namespace gpcl

#endif // GPCL_POOL_HPP
