//
// src.hpp
// ~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define GPCL_SOURCE 1

#ifndef GPCL_SEPARATE_COMPILATION
#error "GPCL_SEPERATE_COMPILATION is not defined"
#endif

#include <gpcl/detail/config.hpp>
#include <gpcl/detail/impl/error.ipp>
#include <gpcl/detail/impl/unreachable.ipp>

#ifdef GPCL_POSIX
#include <gpcl/detail/impl/posix_clock.ipp>
#include <gpcl/detail/impl/posix_condition_variable.ipp>
#include <gpcl/detail/impl/posix_mutex.ipp>
#include <gpcl/detail/impl/posix_semaphore.ipp>
#include <gpcl/detail/impl/posix_thread.ipp>
#include <gpcl/detail/impl/posix_timer.ipp>
#include <gpcl/detail/impl/unique_file_descriptor.ipp>
#endif

#ifdef GPCL_WINDOWS
#include <gpcl/detail/impl/unique_handle.ipp>
#include <gpcl/detail/impl/win_clock.ipp>
#include <gpcl/detail/impl/win_condition_variable.ipp>
#include <gpcl/detail/impl/win_mutex.ipp>
#include <gpcl/detail/impl/win_semaphore.ipp>
#include <gpcl/detail/impl/win_thread.ipp>
#include <gpcl/detail/impl/win_lock_file.ipp>
#endif
