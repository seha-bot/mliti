# TODO add sanitizers and cppcheck

cmake_minimum_required(VERSION 3.23)
project(mliti VERSION 0.0.1 LANGUAGES CXX)

# disable standard decaying and disable extensions
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

option(BUILD_TESTS "should tests be built" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
