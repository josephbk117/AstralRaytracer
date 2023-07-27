cmake_minimum_required(VERSION 3.19.4)
project(CmakeCppLearn)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add the main.cpp and all collected source files to the source files of your executable
add_executable(${PROJECT_NAME} ./src/main.cpp ${HEADER_FILES} ${SOURCE_FILES})
include_directories("./includes/WindowFramework")

add_subdirectory(external/glfw)
add_subdirectory(external/glBinding)

target_include_directories(${PROJECT_NAME} 
	PUBLIC includes/ 
	PUBLIC external/glfw/include
	PUBLIC external/glBinding/source/glbinding/include
)

target_link_directories(${PROJECT_NAME}
	PRIVATE external/glfw/src
	PRIVATE external/glBinding/source/glbinding/source
)

target_link_libraries(${PROJECT_NAME} glfw glbinding)
#target_link_libraries(${PROJECT_NAME} PUBLIC
    #glbinding::glbinding
    #glbinding::glbinding-aux # for additional, auxiliary features as logging, meta information, or debugging functionality
#)