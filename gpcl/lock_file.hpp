#ifndef GPCL_LOCK_FILE_HPP
#define GPCL_LOCK_FILE_HPP

#include <gpcl/detail/config.hpp>

#if defined(GPCL_WINDOWS)
#include <gpcl/detail/win_lock_file.hpp>
#elif defined(GPCL_POSIX)
#include <gpcl/detail/posix_lock_file.hpp>
#endif

namespace gpcl
{
#if defined(GPCL_DOXYGEN)
  class lock_file : noncopyable {
  public:
    explicit lock_file(std::string filename);

    ~lock_file();

    void lock();

    void try_lock();

    void unlock();
  };
#elif defined(GPCL_WINDOWS)
  typedef detail::win_lock_file lock_file;
#elif defined(GPCL_POSIX)
  typedef detail::posix_lock_file lock_file;
#endif  
}

#endif
