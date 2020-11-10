//
// win_thread.ipp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/detail/error.hpp>
#include <gpcl/detail/win_thread.hpp>
#include <iostream>
#include <memory>

#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
#include <boost/exception/diagnostic_information.hpp>
#endif

#ifdef GPCL_WINDOWS
#include <process.h>

namespace gpcl {
namespace detail {

auto __stdcall win_thread_proc(void *arg) -> unsigned {
  GPCL_ASSERT(arg != nullptr);
  auto fn = std::unique_ptr<win_thread::func_base>(
      reinterpret_cast<win_thread::func_base *>(arg));
#ifdef GPCL_USE_BOOST_SYSTEM_ERROR
  try {
    fn->run();
  } catch (...) {
    std::cerr << boost::current_exception_diagnostic_information() << std::endl;
    std::terminate();
  }
#else
  try {
    fn->run();
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    std::terminate();
  }
#endif
  ::ExitThread(0);
  return 0;
}

win_thread::~win_thread() {
  GPCL_ASSERT(!joinable() && "thread has not been joined");
}

auto win_thread::join() -> void {
  GPCL_ASSERT(joinable());

  switch (::WaitForSingleObject(thread_.get(), INFINITE)) {
  case WAIT_OBJECT_0:
    thread_.reset();
    return;

  case WAIT_FAILED:
    throw_system_error("WaitForSingleObject");

  default:
    GPCL_UNREACHABLE("unexpected return value");
  }
}

auto win_thread::detach() -> void { thread_.reset(); }

auto win_thread::start_thread_impl(func_base *fn) -> void {
  thread_ = null_handle{
      (::HANDLE)::_beginthreadex(nullptr, 0, win_thread_proc, fn, 0, nullptr)};

  if (!thread_) {
    DWORD err = ::GetLastError();
    delete fn;
    throw_system_error(err, "CreateThread");
  }
}

} // namespace detail
} // namespace gpcl

#endif
