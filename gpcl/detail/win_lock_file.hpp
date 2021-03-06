#ifndef GPCL_DETAIL_WIN_LOCK_FILE_HPP
#define GPCL_DETAIL_WIN_LOCK_FILE_HPP

#include <gpcl/detail/config.hpp>
#include <gpcl/noncopyable.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/assert.hpp>
#include <Windows.h>

namespace gpcl {
  namespace detail {
    class win_lock_file : noncopyable {
    public:
      explicit win_lock_file(std::string filename)
        : filename_(std::move(filename)),
          handle_(INVALID_HANDLE_VALUE)
      {
      }

      ~win_lock_file() {
        if (is_locked()) {
          unlock();
        }
      }

      bool is_locked() const {
        return handle_ != INVALID_HANDLE_VALUE;
      }

      GPCL_DECL void lock();

      GPCL_DECL bool try_lock();
      
      GPCL_DECL void unlock();
      
    private:
      std::string filename_;
      ::HANDLE handle_;
    };
  }
}

#ifdef GPCL_HEADER_ONLY
#include <gpcl/detail/impl/win_lock_file.ipp>
#endif

#endif
