project(google_tests)
add_subdirectory(lib)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(SOURCE_FILES
        netsim_test.cpp
        netsim_tests/netsim_tests/test/test_package.cpp)

set(HEADER_FILES
        netsim_tests/netsim_tests/include/config.hpp)

add_library(include_tests STATIC ${SOURCE_FILES} ${HEADER_FILES})

add_executable(Google_Tests_Run ${SOURCE_FILES})



target_link_libraries(Google_Tests_Run gtest gtest_main)