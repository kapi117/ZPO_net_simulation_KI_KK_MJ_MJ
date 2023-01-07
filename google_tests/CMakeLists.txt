project(google_tests)
add_subdirectory(lib)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_Run netsim_test.cpp)

target_link_libraries(Google_Tests_Run include)


target_link_libraries(Google_Tests_Run gtest gtest_main)