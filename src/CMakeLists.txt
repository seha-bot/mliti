# Common compiler options
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_23)
target_compile_options(compiler_flags INTERFACE -Wall -Wextra -Wpedantic -Wunused -Wconversion -Wsign-conversion)

# Core library for linking with all targets
add_library(mliti_core INTERFACE)
target_sources(mliti_core
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
)
target_link_libraries(mliti_core INTERFACE compiler_flags)

include(FetchContent)

FetchContent_Declare(argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG d924b84eba1f0f0adf38b20b7b4829f6f65b6570
)
FetchContent_MakeAvailable(argparse)

add_executable(mliti main.cpp)
target_link_libraries(mliti PRIVATE mliti_core argparse)
