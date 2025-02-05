cmake_minimum_required(VERSION 3.16)
project(gamb)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

add_executable(gamb src/main.c src/states/State.c src/states/GameState.c src/Collision.c)

target_include_directories(gamb PRIVATE src)
target_link_libraries(gamb PRIVATE SDL2 m)
