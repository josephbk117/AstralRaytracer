cmake_minimum_required(VERSION 3.19.4)
project(AstralRaytracer)

# Set C++ standard to C++20 and disable extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the subdirectories for the library and the application
add_subdirectory(AstralRaytracerLib)
add_subdirectory(AstralRaytracerApp)

# Glob source and header files
#file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
#file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
#file(GLOB IMGUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*")
#file(GLOB IMGUIZMO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/imguizmo/*")
#file(GLOB STB_IMAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/stbimage/*")
#file(GLOB GLM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/glm/*")
#
## Create the executable and add source/header files
#add_executable (${PROJECT_NAME}
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
#    ${HEADER_FILES}
#    ${SOURCE_FILES}
#    ${IMGUI_FILES}
#    ${IMGUIZMO_FILES}
#    ${STB_IMAGE_FILES}
#    ${GLM_FILES}
#)
#
#Download all submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()
#
#
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(external/tinygltf)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)
set(OPTION_BUILD_TOOLS OFF)
set(OPTION_BUILD_EXAMPLES OFF)
set(OPTION_BUILD_WITH_LTO ON)
add_subdirectory(external/glBinding EXCLUDE_FROM_ALL)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(external/yamlcpp EXCLUDE_FROM_ALL)
add_subdirectory(external/stduuid EXCLUDE_FROM_ALL)
#
## Add resources as a subdirectory
#add_subdirectory(resources)
#
## Include directories for the main project target
#target_include_directories(${PROJECT_NAME}
#    PUBLIC includes/
#    PUBLIC external/glfw/include
#    PUBLIC external/glBinding/source/glbinding/include
#    PUBLIC external/imgui
#    PUBLIC external/imguizmo
#    PUBLIC external/glm
#    PUBLIC external/tinygltf
#    PUBLIC external/yamlcpp/include
#    PUBLIC external/stduuid/include
#)
#
## Link directories for the main project target
#target_link_directories(${PROJECT_NAME}
#    PRIVATE external/glfw/src
#    PRIVATE external/glBinding/source/glbinding/source
#    PRIVATE external/yamlcpp/src
#)
#
# Link libraries for the main project target
#target_link_libraries(${PROJECT_NAME}
#    glfw
#    glbinding
#    yaml-cpp
#)
#
## Custom target for copying resources
#add_custom_target(copy_resources ALL
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${PROJECT_SOURCE_DIR}/resources
#        ${PROJECT_BINARY_DIR}/resources
#    COMMENT "Copying resources into the binary directory"
#)
#
## Add a dependency on copying resources
#add_dependencies(${PROJECT_NAME} copy_resources)
#
## Enable SIMD optimizations only in Release build type
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#
#        message("Setting compiler options for GNU/Clang")
#        # Enable SIMD optimizations for your target architecture.
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
#        # Add other optimization flags as needed.
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#         # Disable RTTI.
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
#    endif()
#
#    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#
#        message("Setting compiler options for MSVC")
#        # Enable SIMD optimizations for your target architecture.
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
#        # Add other optimization flags as needed.
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
#         # Disable RTTI.
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
#    endif()
#endif()