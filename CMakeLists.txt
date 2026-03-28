cmake_minimum_required(VERSION 3.23)

project(Crossword)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(Crossword ${SOURCES})

# Include directories
target_include_directories(Crossword PRIVATE src/include)
