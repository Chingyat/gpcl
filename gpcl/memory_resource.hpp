#ifndef GPCL_MEMORY_RESOURCE_HPP
#define GPCL_MEMORY_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/buffer.hpp>
#include <cstddef> // for max_align_t

namespace gpcl {
inline namespace pmr {

class memory_resource
{
public:
  virtual ~memory_resource() = default;

  [[nodiscard]] void *
  allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    void *ret = do_allocate(bytes, alignment);
    GPCL_VERIFY(reinterpret_cast<std::uintptr_t>(ret) % alignment == 0);
    return ret;
  }

  void deallocate(void *p, std::size_t bytes,
                  std::size_t alignment = alignof(std::max_align_t))
  {
    GPCL_ASSERT(reinterpret_cast<std::uintptr_t>(p) % alignment == 0);
    return do_deallocate(p, bytes, alignment);
  }

  bool is_equal(const memory_resource &other) const noexcept
  {
    return do_is_equal(other);
  }

private:
  virtual void *do_allocate(std::size_t bytes, std::size_t alignment) = 0;

  virtual void do_deallocate(void *p, std::size_t bytes,
                             std::size_t alignment) = 0;

  virtual bool do_is_equal(const memory_resource &other) const noexcept = 0;
};

GPCL_DECL memory_resource *new_delete_resource() noexcept;

GPCL_DECL memory_resource *null_memory_resource() noexcept;

GPCL_DECL memory_resource *get_default_resource() noexcept;

GPCL_DECL void set_default_resource(memory_resource *resource) noexcept;

class monotonic_buffer_resource : public memory_resource
{
public:
  monotonic_buffer_resource(void *buffer, std::size_t size,
                            memory_resource *upstream = get_default_resource())
      : buffer_(reinterpret_cast<char *>(buffer), size),
        upstream_(upstream)
  {
    GPCL_ASSERT(buffer);
    GPCL_ASSERT(upstream);
  }

  explicit monotonic_buffer_resource(
      std::size_t initial_buffer,
      memory_resource *upstream = get_default_resource())
      : upstream_(upstream),
        next_buffer_bytes_(initial_buffer)
  {
    GPCL_ASSERT(initial_buffer != 0);
    GPCL_ASSERT(upstream);
    request_from_upstream();
  }

  explicit monotonic_buffer_resource(
      memory_resource *upstream = get_default_resource())
      : monotonic_buffer_resource(32, upstream)
  {
  }

  ~monotonic_buffer_resource()
  {
    release();
  }

  void release()
  {
    while (block_list_.ptr)
    {
      block_info next;
      std::memcpy(&next,
                  reinterpret_cast<char *>(block_list_.ptr) + block_list_.size,
                  sizeof(next));
      upstream_->deallocate(block_list_.ptr, block_list_.size,
                            block_list_.alignment);
      block_list_ = next;
    }

    GPCL_VERIFY(block_list_.ptr == nullptr);
    GPCL_VERIFY(block_list_.size == 0);
  }

  memory_resource* upstream_resource() const
  {
    return upstream_;
  }

private:
  struct block_info
  {
    void *ptr;
    std::size_t size;
    std::size_t alignment;
  };

  void *alloc_from_buffer(std::size_t bytes, std::size_t alignment)
  {
    std::uintptr_t off =
        reinterpret_cast<std::uintptr_t>(buffer_.data()) % alignment;
    off = (alignment - off) % alignment;

    if (off > buffer_.size_bytes())
      return nullptr;
    buffer_ += off;
    if (buffer_.size_bytes() < bytes)
      return nullptr;
    auto *ret = buffer_.data();
    buffer_ += bytes;
    return ret;
  }

  void request_from_upstream()
  {
    auto *p = upstream_->allocate(next_buffer_bytes_ + sizeof(block_info));
    std::memcpy(reinterpret_cast<char *>(p) + next_buffer_bytes_, &block_list_,
                sizeof block_list_);
    block_list_.ptr = p;
    block_list_.size = next_buffer_bytes_;
    block_list_.alignment = sizeof(std::max_align_t);
    buffer_ = buffer(p, next_buffer_bytes_);
    next_buffer_bytes_ *= 2;
  }

  void *do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    if (auto ret = alloc_from_buffer(bytes, alignment))
      return ret;

    if (next_buffer_bytes_ < bytes)
      next_buffer_bytes_ = (bytes + sizeof(std::max_align_t) - 1) /
                           sizeof(std::max_align_t) * sizeof(std::max_align_t);

    request_from_upstream();

    return alloc_from_buffer(bytes, alignment);
  }

  void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override
  {
    (void)p;
    (void)bytes;
    (void)alignment;
  }

  bool do_is_equal(const memory_resource &other) const noexcept override
  {
    return &other == this;
  }

  mutable_buffer buffer_;
  memory_resource *upstream_;
  std::size_t next_buffer_bytes_ = 32;
  block_info block_list_{};
};
} // namespace pmr

} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/impl/memory_resource.hpp>
#endif

#endif // GPCL_MEMORY_RESOURCE_HPP
