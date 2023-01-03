cmake_minimum_required(VERSION 3.13)
project(implementation)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include
)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        )

set(HEADER_FILES
        include/types.hpp
        include/storage_types.hpp
        include/package.hpp
        )

add_subdirectory(google_tests)
add_library(include STATIC ${SOURCE_FILES} ${HEADER_FILES})

add_executable(netsim ${SOURCE_FILES} main.cpp)