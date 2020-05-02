cmake_minimum_required(VERSION 3.15)
project(corespot
    VERSION 0.0.3
    DESCRIPTION "C++ generic library coded for learning purposes"
    LANGUAGES C CXX
)

set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${MODULE_PATH})
include(${MODULE_PATH}/conan.cmake)

# Includes
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INCLUDES
    ${INCLUDE_DIR}/spot/handle.h
    ${INCLUDE_DIR}/spot/hash.h
    ${INCLUDE_DIR}/spot/log.h
)
source_group(include FILES ${INCLUDES})

# Sources
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES
    ${SOURCE_DIR}/log.cc
)
source_group(src FILES ${SOURCES})

# Library
add_library(${PROJECT_NAME} ${INCLUDES} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::fmt)

# Test
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
