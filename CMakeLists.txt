cmake_minimum_required(VERSION 3.10)
project(OpenGLWin32)

# Enable C++ Standard
set(CMAKE_CXX_STANDARD 11)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add executable
add_executable(OpenGLWin32 
    main.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Implementation/RenderPlayer.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Implementation/RenderWorld.c
)

# Include directories (for OpenGL and headers)
include_directories(
    ${OPENGL_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # Ensure this is where your headers are
    ${CMAKE_CURRENT_SOURCE_DIR}/src      # Add src directory for .h files
)

# Add stb_image (ensure stb_image.h is in the "include" folder)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(OpenGLWin32 
    ${OPENGL_LIBRARIES}   # OpenGL
    glu32                 # GL Utility Library
    gdi32                 # Windows GDI
)
