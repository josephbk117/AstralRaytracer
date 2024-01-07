file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IMGUI_FILES "${PROJECT_SOURCE_DIR}/external/imgui/*")

# Add resources as a subdirectory
add_subdirectory(app_assets)

# Custom target for copying resources
add_custom_target(copy_app_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/app_assets
        ${CMAKE_CURRENT_BINARY_DIR}/app_assets
    COMMENT "Copying app assets into the binary directory"
)

# Define the executable and link it with the library
add_executable(AstralRaytracerApp 
               ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
               ${HEADER_FILES}
               ${SOURCE_FILES}
               ${IMGUI_FILES}
               ${IMGUI_FILE_DIALOG_FILES}
)

# Add a dependency on copying resources
add_dependencies(AstralRaytracerApp copy_app_assets)

# Specify the include directories for the application
target_include_directories(AstralRaytracerApp 
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/include
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/imgui
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/efsw/include
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/mimalloc/include
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/glm
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/nativefiledialog-extended/include
                           PUBLIC ${PROJECT_SOURCE_DIR}/external/hwinfo/include
)

# Link the executable with the library
target_link_libraries(AstralRaytracerApp 
                            PRIVATE AstralRaytracerLib
                            PRIVATE glfw
                            PRIVATE efsw
                            PRIVATE nfd
                            PRIVATE hwinfo::HWinfo
)