file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Define the executable and link it with the library
add_executable(AstralRaytracerCmdTool 
               ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
               ${HEADER_FILES}
               ${SOURCE_FILES}
)

# Specify the include directories for the application
target_include_directories(AstralRaytracerCmdTool
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# Link the executable with the library
target_link_libraries(AstralRaytracerCmdTool 
                            PRIVATE AstralRaytracerLib
)