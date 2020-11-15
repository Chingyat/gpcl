include(CTest)
include(Catch)

add_executable(tests main.cpp expected_test.cpp)
target_link_libraries(tests PRIVATE gpcl::gpcl)

catch_discover_tests(tests)
