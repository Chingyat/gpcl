#
# CMakeLists.txt
# ~~~~~~~~~~~~~~
#
# Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.12)

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
	gpcl/buffer.hpp
	gpcl/buffer_sequence.hpp
	gpcl/clock.hpp
	gpcl/clone_ptr.hpp
	gpcl/creation_tag.hpp
	gpcl/decay_copy.hpp
	gpcl/detail/chrono.hpp
	gpcl/detail/expected.hpp
	gpcl/detail/impl/error.ipp
	gpcl/detail/impl/posix_clock.ipp
	gpcl/detail/impl/posix_message_queue.ipp
	gpcl/detail/impl/posix_mutex.ipp
	gpcl/detail/impl/posix_semaphore.ipp
	gpcl/detail/impl/posix_thread.ipp
	gpcl/detail/impl/unique_file_descriptor.ipp
	gpcl/detail/impl/unique_handle.ipp
	gpcl/detail/impl/unreachable.ipp
	gpcl/detail/impl/win_semaphore.ipp
	gpcl/detail/impl/win_thread.ipp
	gpcl/detail/impl/posix_condition_variable.ipp
	gpcl/detail/impl/win_clock.ipp
	gpcl/detail/impl/win_condition_variable.ipp
	gpcl/detail/impl/win_mutex.ipp
	gpcl/detail/impl/win_lock_file.ipp
	gpcl/detail/impl/posix_lock_file.ipp
	gpcl/detail/impl/posix_file.ipp
	gpcl/detail/optional.hpp
	gpcl/detail/posix_clock.hpp
	gpcl/detail/posix_mutex.hpp
	gpcl/detail/posix_semaphore.hpp
	gpcl/detail/posix_thread.hpp
	gpcl/detail/thread_annotations.hpp
	gpcl/detail/type_traits.hpp
	gpcl/detail/unique_file_descriptor.hpp
	gpcl/detail/unreachable.hpp
	gpcl/detail/utility.hpp
	gpcl/detail/win_clock.hpp
	gpcl/detail/config.hpp
	gpcl/detail/error.hpp
	gpcl/detail/posix_condition_variable.hpp
	gpcl/detail/posix_message_queue.hpp
	gpcl/detail/unique_handle.hpp
	gpcl/detail/win_condition_variable.hpp
	gpcl/detail/win_mutex.hpp
	gpcl/detail/win_semaphore.hpp
	gpcl/detail/win_thread.hpp
	gpcl/detail/win_lock_file.hpp
	gpcl/detail/posix_lock_file.hpp
	gpcl/detail/posix_file.hpp
	gpcl/error.hpp
	gpcl/event.hpp
	gpcl/expected_fwd.hpp
	gpcl/impl/offset_ptr.hpp
	gpcl/impl/expected.hpp
	gpcl/impl/src.hpp
	gpcl/in_place.hpp
	gpcl/is_basic_lockable.hpp
	gpcl/is_lockable.hpp
	gpcl/mutex.hpp
	gpcl/noncopyable.hpp
	gpcl/object_pool.hpp
	gpcl/offset_ptr.hpp
	gpcl/optional_fwd.hpp
	gpcl/pmr/default_resource.hpp
	gpcl/pmr/impl/default_resource.ipp
	gpcl/pmr/impl/monotonic_buffer_resource.ipp
	gpcl/pmr/impl/null_memory_resource.ipp
	gpcl/pmr/impl/new_delete_resource.ipp
	gpcl/pmr/memory_resource.hpp
	gpcl/pmr/monotonic_buffer_resource.hpp
	gpcl/pmr/new_delete_resource.hpp
	gpcl/pmr/null_memory_resource.hpp
	gpcl/pmr/polymorphic_allocator.hpp
	gpcl/pool_allocator.hpp
	gpcl/semaphore.hpp
	gpcl/span.hpp
	gpcl/thread.hpp
	gpcl/thread_annotations.hpp
	gpcl/thread_attributes.hpp
	gpcl/unexpected.hpp
	gpcl/unique_resource.hpp
	gpcl/version.hpp
	gpcl/zstring.hpp
	gpcl/assert.hpp
	gpcl/condition_variable.hpp
	gpcl/expected.hpp
	gpcl/inttypes.hpp
	gpcl/message_queue.hpp
	gpcl/narrow_cast.hpp
	gpcl/optional.hpp
	gpcl/pool.hpp
	gpcl/simple_segregated_storage.hpp
	gpcl/time.hpp
	gpcl/unique_lock.hpp
	gpcl/lock_file.hpp
	gpcl/file.hpp
	gpcl/intrusive_list.hpp
    )


add_library(gpcl INTERFACE)
add_library(gpcl::gpcl ALIAS gpcl)
target_include_directories(gpcl 
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)
if (UNIX)
	target_link_libraries(gpcl
		INTERFACE
		pthread
		)
endif ()

install(DIRECTORY gpcl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES gpcl.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(CATCH_BUILD_TESTING OFF)
add_subdirectory(vendor/Catch2)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2/extras")

enable_testing()
include(CTest)
include(Catch)

add_executable(tests
        ${GPCL_HEADERS}
		tests/include_src.cpp
        tests/expected_test.cpp
        tests/monotonic_buffer_resource_test.cpp
		tests/time_test.cpp
		tests/file_test.cpp
        tests/lock_file_test.cpp
		tests/unique_resource_test.cpp
        )
target_link_libraries(tests PRIVATE gpcl::gpcl Catch2::Catch2WithMain)

catch_discover_tests(tests)

configure_file(tools/standardese.ini.in standardese.in)
configure_file(tools/build_doc.cmake.in build_doc.cmake @ONLY)
configure_file(tools/add_copyright.cmake.in add_copyright.cmake @ONLY)
