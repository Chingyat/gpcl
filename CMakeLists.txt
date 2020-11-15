#
# CMakeLists.txt
# ~~~~~~~~~~~~~~
#
# Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.13)

project(GPCL CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

if (MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(GPCL_ENABLE_TESTS "enable tests" ON)
option(GPCL_ENABLE_BENCHMARK "enable benchmark" ON)
option(GPCL_ENABLE_BOOST "link to boost" OFF)
option(GPCL_SEPARATE_COMPILATION "build GPCL as a static/dynamic library" OFF)
option(GPCL_ENABLE_DOC "Build documentation" OFF)

find_package(
        Boost
        COMPONENTS system
        QUIET)

enable_testing()

set(GPCL_HEADERS
        gpcl.hpp
        gpcl/assert.hpp
        gpcl/clock.hpp
        gpcl/condition_variable.hpp
        gpcl/detail/chrono.hpp
        gpcl/detail/config.hpp
        gpcl/detail/error.hpp
        gpcl/detail/expected.hpp
        gpcl/detail/impl/error.ipp
        gpcl/detail/impl/posix_clock.ipp
        gpcl/detail/impl/posix_condition_variable.ipp
        gpcl/detail/impl/posix_mutex.ipp
        gpcl/detail/impl/posix_semaphore.ipp
        gpcl/detail/impl/posix_thread.ipp
        gpcl/detail/impl/posix_timer.ipp
        gpcl/detail/impl/unique_file_descriptor.ipp
        gpcl/detail/impl/unique_handle.ipp
        gpcl/detail/impl/unreachable.ipp
        gpcl/detail/impl/win_clock.ipp
        gpcl/detail/impl/win_condition_variable.ipp
        gpcl/detail/impl/win_mutex.ipp
        gpcl/detail/impl/win_semaphore.ipp
        gpcl/detail/impl/win_thread.ipp
        gpcl/detail/impl/posix_message_queue.ipp
        gpcl/detail/optional.hpp
        gpcl/detail/posix_clock.hpp
        gpcl/detail/posix_condition_variable.hpp
        gpcl/detail/posix_mutex.hpp
        gpcl/detail/posix_semaphore.hpp
        gpcl/detail/posix_thread.hpp
        gpcl/detail/posix_timer.hpp
        gpcl/detail/thread_annotations.hpp
        gpcl/detail/type_traits.hpp
        gpcl/detail/unique_file_descriptor.hpp
        gpcl/detail/unique_handle.hpp
        gpcl/detail/unreachable.hpp
        gpcl/detail/utility.hpp
        gpcl/detail/win_clock.hpp
        gpcl/detail/win_condition_variable.hpp
        gpcl/detail/win_mutex.hpp
        gpcl/detail/win_semaphore.hpp
        gpcl/detail/win_thread.hpp
        gpcl/detail/posix_message_queue.hpp
        gpcl/error.hpp
        gpcl/event.hpp
        gpcl/expected.hpp
        gpcl/expected_fwd.hpp
        gpcl/ext/detail/config.hpp
        gpcl/ext/impl/mqtt.ipp
        gpcl/ext/impl/sqlite.ipp
        gpcl/ext/mqtt.hpp
        gpcl/ext/sqlite.hpp
        gpcl/impl/expected.hpp
        gpcl/impl/ext-src.hpp
        gpcl/impl/offset_ptr.hpp
        gpcl/impl/src.hpp
        gpcl/in_place_t.hpp
        gpcl/is_basic_lockable.hpp
        gpcl/is_lockable.hpp
        gpcl/mutex.hpp
        gpcl/narrow_cast.hpp
        gpcl/noncopyable.hpp
        gpcl/offset_ptr.hpp
        gpcl/optional.hpp
        gpcl/optional_fwd.hpp
        gpcl/semaphore.hpp
        gpcl/span.hpp
        gpcl/thread.hpp
        gpcl/thread_annotations.hpp
        gpcl/thread_attributes.hpp
        gpcl/timer.hpp
        gpcl/unexpected.hpp
        gpcl/unique_lock.hpp
        gpcl/unique_resource.hpp
        gpcl/zstring.hpp
        gpcl/message_queue.hpp
        gpcl/creation_tag.hpp
        gpcl/ext.hpp
        gpcl/buffer.hpp
        gpcl/simple_segregated_storage.hpp
        gpcl/pool_allocator.hpp)

add_library(gpcl INTERFACE)
add_library(gpcl::gpcl ALIAS gpcl)
target_include_directories(gpcl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(DIRECTORY include/gpcl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES include/gpcl.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2")
    set(CATCH_BUILD_TESTING OFF)
    add_subdirectory(vendor/Catch2)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2/extras")
else ()
    find_package(Catch2 REQUIRED)
endif ()

add_subdirectory(tests)
