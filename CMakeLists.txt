cmake_minimum_required(VERSION 3.19.4)
project(CmakeCppLearn)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add the main.cpp and all collected source files to the source files of your executable
add_executable(YourExecutableName ./src/main.cpp ${HEADER_FILES} ${SOURCE_FILES})
include_directories("./includes/WindowFramework")