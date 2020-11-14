//
// object_pool.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_OBJECT_POOL_HPP
#define GPCL_OBJECT_POOL_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/pool.hpp>
#include <gpcl/simple_segregated_storage.hpp>

namespace gpcl {

template <typename T, typename UA = default_new_delete_user_allocator,
          typename MutexType = gpcl::mutex>
class object_pool
{
  struct chunk_info
  {
    T elem;
    chunk_info *prev;
    chunk_info *next;
  };

public:
  using element_type = T;
  using user_allocator = UA;
  using mutex_type = MutexType;
  using size_type = typename user_allocator::size_type;
  using difference_type = typename user_allocator::difference_type;

  object_pool() : pool_(sizeof(chunk_info)), obj_list_() {}

  ~object_pool()
  {
    while (obj_list_)
    {
      obj_list_->elem.~element_type();
      obj_list_ = obj_list_->next;
    }
  }

  element_type *malloc()
  {
    gpcl::unique_lock<mutex_type> lock(mutex_);

    auto *obj = reinterpret_cast<chunk_info *>(pool_.malloc());
    obj->prev = nullptr;
    obj->next = obj_list_;
    if (obj_list_)
      obj_list_->prev = obj;
    obj_list_ = obj;
    return &obj->elem;
  }

  void free(element_type *p)
  {
    GPCL_ASSERT(p != nullptr);
    auto *obj = reinterpret_cast<chunk_info *>(p);
    gpcl::unique_lock<mutex_type> lock(mutex_);

    auto prev = obj->prev;
    auto next = obj->next;
    if (prev)
      prev->next = next;
    if (next)
      next->prev = prev;
    pool_.free(p);
    if (obj_list_ == obj)
      obj_list_ = next;
  }

  bool is_from(element_type *p) { GPCL_UNIMPLEMENTED(); }

  element_type *construct() { return new (malloc()) element_type(); }

  template <typename... Args>
  element_type *construct(Args &&... args)
  {
    return new (malloc()) element_type(std::forward<Args>(args)...);
  }

  void destroy(element_type *p)
  {
    p->~element_type();
    free(p);
  }

  size_type get_next_size() const { pool_.get_next_size(); }

  void set_next_size(size_type next_size) { pool_.set_next_size(next_size); }

private:
  pool<user_allocator, null_mutex> pool_;
  mutex_type mutex_;
  chunk_info *obj_list_;
};

} // namespace gpcl
#endif // GPCL_OBJECT_POOL_HPP
