cmake_minimum_required(VERSION 3.19)
project(mliti
	VERSION 0.0.1
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG 914aeecfe23b1e16af6ea675a4fb5dbd5a5b8d0a
	SOURCE_DIR Catch2
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(mliti
	src/main.cpp
)

target_include_directories(mliti PRIVATE
	# ${CMAKE_BINARY_DIR}/cxxopts_git/include

	src/include
)

set(BASE_FLAGS
	-Wall -Wextra -Wpedantic -Wunused -Wconversion
	-fsanitize=address,undefined
)

target_compile_options(mliti PRIVATE ${BASE_FLAGS} -g -O0)

target_link_libraries(mliti PRIVATE
	# taocpp::pegtl
	-fsanitize=address,undefined
	argparse
)

# Tests
add_executable(tests src/test.cpp)
target_include_directories(tests PRIVATE src/include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
