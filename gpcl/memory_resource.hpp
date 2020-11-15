#ifndef GPCL_MEMORY_RESOURCE_HPP
#define GPCL_MEMORY_RESOURCE_HPP

#include <gpcl/detail/config.hpp>
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
    return do_allocate(bytes, alignment);
  }

  void deallocate(void *p, std::size_t bytes,
                  std::size_t alignment = alignof(std::max_align_t))
  {
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

// fixme: do not allocate memory for storing block info.
class monotonic_buffer_resource : public memory_resource
{
public:
  monotonic_buffer_resource(void *buffer, std::size_t size,
                            memory_resource *upstream = new_delete_resource())
      : buffer_(reinterpret_cast<char *>(buffer), size),
        upstream_(upstream)
  {
  }

  explicit monotonic_buffer_resource(
      std::size_t initial_buffer,
      memory_resource *upstream = new_delete_resource())
      : upstream_(upstream),
        next_buffer_bytes_(initial_buffer)
  {
    request_from_upstream();
  }

  explicit monotonic_buffer_resource(
      memory_resource *upstream = new_delete_resource())
      : monotonic_buffer_resource(32, upstream)
  {
  }

  ~monotonic_buffer_resource()
  {
    for (auto block : blocks_)
      upstream_->deallocate(std::get<0>(block), std::get<1>(block),
                            std::get<2>(block));
  }

private:
  void *alloc_from_buffer(std::size_t bytes, std::size_t alignment)
  {
    std::uintptr_t off =
        reinterpret_cast<std::uintptr_t>(buffer_.data()) % alignment;
    if (off > buffer_.size_bytes())
      return nullptr;
    buffer_ = buffer_.subspan(off);
    if (buffer_.size_bytes() < bytes)
      return nullptr;
    auto *ret = buffer_.data();
    buffer_ = buffer_.subspan(bytes);
    return ret;
  }

  void request_from_upstream()
  {
    auto *p = upstream_->allocate(next_buffer_bytes_);
    blocks_.emplace_back(p, next_buffer_bytes_, sizeof(std::max_align_t));
    buffer_ = span<char>(reinterpret_cast<char *>(p), next_buffer_bytes_);
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

  span<char> buffer_;
  memory_resource *upstream_;
  std::size_t next_buffer_bytes_ = 32;
  std::vector<std::tuple<void *, std::size_t, std::size_t>> blocks_;
};
} // namespace pmr

} // namespace gpcl

#ifdef GPCL_HEADER_ONLY
#  include <gpcl/impl/memory_resource.hpp>
#endif

#endif // GPCL_MEMORY_RESOURCE_HPP
