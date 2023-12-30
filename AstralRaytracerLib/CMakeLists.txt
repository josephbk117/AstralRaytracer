# Glob source and header files
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB STB_IMAGE_FILES "${PROJECT_SOURCE_DIR}/external/stbimage/*")

# Define the library
add_library(AstralRaytracerLib STATIC
            ${HEADER_FILES}
            ${SOURCE_FILES}
            ${STB_IMAGE_FILES}
)

# Specify the include directories for this library
target_include_directories(AstralRaytracerLib 
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/includes"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/stduuid/include"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/glBinding/source/glbinding/include"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/yamlcpp/include"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/spdlog/include"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/mimalloc/include"
                           PUBLIC "${PROJECT_SOURCE_DIR}/external/glm"
)

target_link_libraries(AstralRaytracerLib
    glbinding
    yaml-cpp
    spdlog
    mimalloc-static
    glm::glm
)