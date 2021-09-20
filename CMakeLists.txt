cmake_minimum_required(VERSION 3.16)

project(random-container CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-fsanitize=address -fsanitize=undefined)
add_link_options(-fsanitize=address -fsanitize=undefined)

include_directories(include)

add_executable(random-container-tests src/random_container_tests.cpp 
                                      src/doctest_main.cpp)