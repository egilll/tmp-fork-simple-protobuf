cmake_minimum_required(VERSION 3.30)

project(parser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
        pegtl
        GIT_REPOSITORY https://github.com/taocpp/PEGTL.git
        GIT_TAG        3.2.8
)
FetchContent_MakeAvailable(pegtl)

add_compile_options(-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=undefined,address)
    add_link_options(-fsanitize=undefined,address)
endif ()

add_executable(parser main.cpp)
target_link_libraries(parser PUBLIC pegtl)