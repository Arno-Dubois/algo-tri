cmake_minimum_required(VERSION 3.16)
project(algo_tri LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(ALGO_TRI_WARNINGS -Wall -Wextra -Wpedantic)
endif()

# Core logic: plain C, no SDL dependency, unit-tested
add_library(depot STATIC src/depot.c)
target_include_directories(depot PUBLIC src)
target_compile_options(depot PRIVATE ${ALGO_TRI_WARNINGS})

# Graphical application
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)

add_executable(algo_tri src/main.c)
target_compile_options(algo_tri PRIVATE ${ALGO_TRI_WARNINGS})
target_link_libraries(algo_tri PRIVATE depot SDL2_ttf::SDL2_ttf SDL2::SDL2)
if(TARGET SDL2::SDL2main)
    target_link_libraries(algo_tri PRIVATE SDL2::SDL2main)
endif()

# Copy the font next to the executable
add_custom_command(TARGET algo_tri POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:algo_tri>/assets)

# Tests
include(CTest)
if(BUILD_TESTING)
    add_executable(test_depot tests/test_depot.c)
    target_compile_options(test_depot PRIVATE ${ALGO_TRI_WARNINGS})
    target_link_libraries(test_depot PRIVATE depot)
    add_test(NAME depot COMMAND test_depot)
endif()
