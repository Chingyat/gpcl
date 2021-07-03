#include <gpcl/detail/posix_lock_file.hpp>
#include <gpcl/unique_lock.hpp>
#include <fcntl.h>
#include <unistd.h>

namespace gpcl {
namespace detail {

void posix_lock_file::lock()
{
  unique_lock<mutex> lk{ mtx_ };

  GPCL_ASSERT(!owns_lock());
  file_.open(open_or_create, filename_.c_str(),
             posix_file::access_mode::readwrite);

  if (::lockf(file_.native_handle(), F_TLOCK, 0) == -1)
  {
    throw_system_error(__func__);
  }

  ::ftruncate(file_.native_handle(), 0);

  char pidb[20];
  int len =
      snprintf(pidb, sizeof(pidb), "%lu", static_cast<unsigned long>(getpid()));

  ::write(file_.native_handle(), pidb, len);

  owns_lock_ = true;
}

bool posix_lock_file::try_lock()
{
  unique_lock<mutex> lk{ mtx_ };

  GPCL_ASSERT(!owns_lock());
  file_.open(open_or_create, filename_.c_str(),
             posix_file::access_mode::readwrite);
  if (lockf(file_.native_handle(), F_TLOCK, 0) == -1)
  {
    int err = errno;
    if (err == EACCES || err == EAGAIN)
    {
      return false;
    }

    throw_system_error(__func__);
  }
  ::ftruncate(file_.native_handle(), 0);

  char pidb[20];
  int len =
      snprintf(pidb, sizeof(pidb), "%lu", static_cast<unsigned long>(getpid()));

  ::write(file_.native_handle(), pidb, len);

  owns_lock_ = true;
  return true;
}

void posix_lock_file::unlock()
{
  unique_lock<mutex> lk{ mtx_ };

  GPCL_ASSERT(owns_lock());
  ::ftruncate(file_.native_handle(), 0);

  if (lockf(file_.native_handle(), F_ULOCK, 0) == -1)
  {
    throw_system_error(__func__);
  }
  file_.close();

  owns_lock_ = false;
}

} // namespace detail
} // namespace gpcl