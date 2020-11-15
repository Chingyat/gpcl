include(CTest)
include(Catch)

add_executable(tests expected_test.cpp)
target_link_libraries(tests PRIVATE gpcl::gpcl Catch2::Catch2WithMain)

catch_discover_tests(tests)
