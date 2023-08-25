cmake_minimum_required(VERSION 3.19.4)
project(AstralRaytracer)

# Set C++ standard to C++17 and disable extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Glob source and header files
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB IMGUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*")
file(GLOB IMGUIZMO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/imguizmo/*")
file(GLOB STB_IMAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/stbimage/*")
file(GLOB GLM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/glm/*")

# Create the executable and add source/header files
add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${IMGUI_FILES}
    ${IMGUIZMO_FILES}
    ${STB_IMAGE_FILES}
    ${GLM_FILES}
)

# Include directories
include_directories(
    "./includes/WindowFramework"
    "./external/imgui"
    "./external/imguizmo"
    "./external/stbimage"
    "./external/glm"
)

# Set TinyGLTF configuration options
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

# Add TinyGLTF as a subdirectory
add_subdirectory(external/tinygltf)

# Add GLFW and glBinding as subdirectories, excluding them from the "ALL" target
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)
add_subdirectory(external/glBinding EXCLUDE_FROM_ALL)

# Add resources as a subdirectory
add_subdirectory(resources)

# Include directories for the main project target
target_include_directories(${PROJECT_NAME}
    PUBLIC includes/
    PUBLIC external/glfw/include
    PUBLIC external/glBinding/source/glbinding/include
    PUBLIC external/tinygltf
)

# Link directories for the main project target
target_link_directories(${PROJECT_NAME}
    PRIVATE external/glfw/src
    PRIVATE external/glBinding/source/glbinding/source
)

# Link libraries for the main project target
target_link_libraries(${PROJECT_NAME}
    glfw
    glbinding
)

# Custom target for copying resources
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
    COMMENT "Copying resources into the binary directory"
)

# Add a dependency on copying resources
add_dependencies(${PROJECT_NAME} copy_resources)

# Enable SIMD optimizations only in Release build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        # Enable SIMD optimizations for your target architecture.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        # Add other optimization flags as needed.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
         # Disable RTTI.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Enable SIMD optimizations for your target architecture.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        # Add other optimization flags as needed.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
         # Disable RTTI.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    endif()

    if(MSVC)
        # Enable SIMD optimizations for your target architecture.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
        # Add other optimization flags as needed.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
         # Disable RTTI.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    endif()
endif()