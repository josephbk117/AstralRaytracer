include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(AstralRaytracerTests tests.cpp)

# Include glm headers
target_include_directories(AstralRaytracerTests
						    PRIVATE ${PROJECT_SOURCE_DIR}/AstralRaytracerLib/includes
						    PRIVATE ${PROJECT_SOURCE_DIR}/external/
                            PRIVATE ${PROJECT_SOURCE_DIR}/external/stduuid/include
                            PRIVATE ${PROJECT_SOURCE_DIR}/external/glBinding/source/glbinding/include
                            PRIVATE ${PROJECT_SOURCE_DIR}/external/yamlcpp/include
                            PRIVATE ${PROJECT_SOURCE_DIR}/external/spdlog/include
                            PRIVATE ${PROJECT_SOURCE_DIR}/external/mimalloc/include
						    PRIVATE ${PROJECT_SOURCE_DIR}/external/glm
)

target_link_libraries(AstralRaytracerTests
                        PRIVATE GTest::GTest
                        PRIVATE AstralRaytracerLib
)

add_test(tests_gtest AstralRaytracerTests)