cmake_minimum_required(VERSION 3.19.4)
project(CmakeCppLearn)
#include_directories(WindowFramework)
#add_executable(CmakeCppLearn main.cpp)
#add_subdirectory(WindowFramework)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add the main.cpp and all collected source files to the source files of your executable
add_executable(YourExecutableName ./src/main.cpp ${HEADER_FILES} ${SOURCE_FILES})

#set(my_includes "./includes/WindowFramework/Window.h") 
#set(my_cpps "./src/main.cpp" "./src/WindowFramework/Window.cpp")
#add_executable(yourExec ${my_includes} ${my_cpps}) 
include_directories("./includes/WindowFramework")