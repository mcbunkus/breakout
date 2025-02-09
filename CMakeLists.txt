cmake_minimum_required(VERSION 3.16)
project(gamb)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)

add_executable(gamb 
  src/Fonts/Square.c
  src/Rectangle.c
  src/State.c
  src/States/GameState.c 
  src/States/MenuState.c 
  src/UI/Button.c
  src/UI/Label.c
  src/UI/UI.c
  src/Util.c
  src/main.c 
)

set(TARGET gamb PROPERTY C_STANDARD 11)

if (TARGET SDL2::SDL2main)
  target_link_libraries(gamb PRIVATE SDL2::SDL2main)
endif()

target_include_directories(gamb PRIVATE src)
target_link_libraries(gamb PRIVATE SDL2 SDL2_ttf m)

