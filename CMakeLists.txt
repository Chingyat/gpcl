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
        gpcl/error.hpp
        gpcl/event.hpp
        gpcl/expected.hpp
        gpcl/expected_fwd.hpp
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

set(CATCH_BUILD_TESTING OFF)
add_subdirectory(vendor/Catch2)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2/extras")

enable_testing()
include(CTest)
include(Catch)

add_executable(tests ${GPCL_HEADERS} tests/expected_test.cpp)
target_link_libraries(tests PRIVATE gpcl::gpcl Catch2::Catch2WithMain)

catch_discover_tests(tests)

configure_file(tools/standardese.ini.in standardese.in)
configure_file(tools/build_doc.cmake.in build_doc.cmake)
