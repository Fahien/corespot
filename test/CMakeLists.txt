# Sources
set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc
)
source_group(test FILES ${TEST_SOURCES})

# Executable
add_executable(test-${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(test-${PROJECT_NAME} CONAN_PKG::catch2 ${PROJECT_NAME})

include(CTest)
include(Catch)
catch_discover_tests(test-${PROJECT_NAME})
