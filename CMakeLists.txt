cmake_minimum_required(VERSION 3.16)
project(gamb)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)

add_executable(gamb 
  src/main.c 
  src/states/State.c 
  src/states/GameState.c 
  src/Collision.c
  src/UI.c)

if (TARGET SDL2::SDL2main)
  target_link_libraries(gamb PRIVATE SDL2::SDL2main)
endif()

target_include_directories(gamb PRIVATE src)
target_link_libraries(gamb PRIVATE SDL2 SDL2_ttf m)

# add_custom_command(
#   TARGET gamb POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E
#   copy_
# )
