cmake_minimum_required(VERSION 3.10)
project(GeometryFactory)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Build shared library
add_library(${PROJECT_NAME} SHARED
    src/main.c
    src/glad.c
)

# Set position independent code flag
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    OpenGL::GL
    glfw
    m  # Math library
)

# Set library output name
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX "lib"
    OUTPUT_NAME "geometryfactory"
)
