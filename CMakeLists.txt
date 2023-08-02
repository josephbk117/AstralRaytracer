cmake_minimum_required(VERSION 3.19.4)
project(AstralRaytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB IMGUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*")
file(GLOB GLM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/glm/*")
# Add the main.cpp and all collected source files to the source files of your executable
add_executable(${PROJECT_NAME} ./src/main.cpp ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_FILES} ${GLM_FILES})
include_directories("./includes/WindowFramework")
include_directories("./external/imgui")
include_directories("./external/glm")

add_subdirectory(external/glfw)
add_subdirectory(external/glBinding)
add_subdirectory(resources/)

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

add_custom_target(copy_resources ALL
				COMMAND ${CMAKE_COMMAND} -E copy_directory
				${PROJECT_SOURCE_DIR}/resources
				${PROJECT_BINARY_DIR}/resources
				COMMENT "copying resources into binary directory"
)

add_dependencies(${PROJECT_NAME} copy_resources)