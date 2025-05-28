cmake_minimum_required(VERSION 3.10)

project(FluidSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Find OpenGL
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenGL::GL)
endif()

# Find GLFW
find_package(GLFW3 REQUIRED)
if (GLFW3_FOUND)
    target_link_libraries(${PROJECT_NAME} glfw)
endif()

# Find GLEW
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
endif()

