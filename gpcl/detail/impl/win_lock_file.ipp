//
// win_lock_file.ipp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2021 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/detail/win_lock_file.hpp>
#include <Windows.h>

namespace gpcl {
namespace detail {

void win_lock_file::lock()
{
  while (!try_lock())
  {
    HANDLE hd = FindFirstChangeNotificationA(filename_.c_str(), FALSE,
                                             FILE_NOTIFY_CHANGE_FILE_NAME);
    if (hd == INVALID_HANDLE_VALUE)
    {
      throw_system_error("FindFirstChangeNotification");
    }

    DWORD r = WaitForSingleObject(hd, INFINITE);
    CloseHandle(hd);

    switch (r)
    {
    case WAIT_OBJECT_0:
      continue;

    case WAIT_FAILED:
      throw_system_error("WaitForSingleObjcet");
      break;

    default:
      GPCL_UNREACHABLE("invalid return value");
    }
  }
}

bool win_lock_file::try_lock()
{
  GPCL_ASSERT(!is_lock());

  handle_ = ::CreateFileA(
      filename_.c_str(),
      GENERIC_READ | GENERIC_WRITE, // both read and write access
      FILE_SHARE_READ, // other process can only open this file for reading
      nullptr, // no security descriptor and child processes cannot inherit the
               // handle
      CREATE_NEW, // fail if already exists
      FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
      nullptr // no template file
  );

  return is_locked();
}

void win_lock_file::unlock()
{
  GPCL_ASSERT(is_lock());
  CloseHandle(handle_);
  handle_ = INVALID_HANDLE_VALUE;
}

} // namespace detail
} // namespace gpcl
