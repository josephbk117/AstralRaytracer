cmake_minimum_required(VERSION 3.19.4)
project(AstralRaytracer)

# Set C++ standard to C++20 and disable extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}, Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Target architecture: ${CMAKE_SYSTEM_PROCESSOR}, Platform: ${CMAKE_SYSTEM_NAME}")

enable_testing()

# Add the subdirectories for the library and the application
add_subdirectory(AstralRaytracerLib)
add_subdirectory(AstralRaytracerApp)
add_subdirectory(AstralRaytracerTests)

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

add_subdirectory(external/hwinfo)
add_subdirectory(external/nativefiledialog-extended)
add_subdirectory(external/glm)
set(MI_WIN_REDIRECT OFF)
set(MI_BUILD_TESTS OFF)
#set(MI_BUILD_OBJECT OFF)
#set(MI_BUILD_SHARED OFF)
add_subdirectory(external/mimalloc)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(external/efsw)
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)
set(TINYGLTF_INSTALL OFF)
add_subdirectory(external/tinygltf)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)
#set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(external/spdlog)

#enabling LTO has issues on clang

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    message("Link Time Optimisation enabled for glBinding")
    set(OPTION_BUILD_WITH_LTO ON)
else()
    message("Link Time Optimisation disabled for glBinding")
    set(OPTION_BUILD_WITH_LTO OFF)
endif()
set(OPTION_BUILD_TOOLS OFF)
set(OPTION_BUILD_EXAMPLES OFF)
add_subdirectory(external/glBinding EXCLUDE_FROM_ALL)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)
set(YAML_CPP_FORMAT_SOURCE OFF)
set(YAML_BUILD_SHARED_LIBS OFF)
add_subdirectory(external/yamlcpp EXCLUDE_FROM_ALL)
add_subdirectory(external/stduuid EXCLUDE_FROM_ALL)

# Enable SIMD optimizations only in Release build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message("Setting compiler options for GNU/Clang")
        # Enable SIMD optimizations for your target architecture.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        # Add other optimization flags as needed.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
         # Disable RTTI.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message("Setting compiler options for MSVC")
        # Enable SIMD optimizations for your target architecture.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
        # Add other optimization flags as needed.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
         # Disable RTTI.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    endif()
endif()