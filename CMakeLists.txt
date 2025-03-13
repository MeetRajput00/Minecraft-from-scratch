cmake_minimum_required(VERSION 3.10)
project(Win32Project)

# Enable C for Windows API
set(CMAKE_C_STANDARD 99)

# Add the executable
add_executable(Win32Project main.c)

# Link against gdi32 for graphics
target_link_libraries(Win32Project gdi32)
