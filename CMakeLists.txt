cmake_minimum_required(VERSION 3.13)
project(bittorrent-starter-cpp)
add_compile_options(-pedantic-errors -Wall -Weffc++ -Wextra  -Wconversion -Wsign-conversion -Werror) 
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

file(GLOB SOURCE_FILES
    src/main.cpp
    src/lib/*/*.cpp
)

add_executable(bittorrent ${SOURCE_FILES})
