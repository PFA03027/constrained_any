cmake_minimum_required(VERSION 3.16)
project(constrained_any CXX)
set(CMAKE_VERBOSE_MAKEFILE 1)


# Switch build target
set(BUILD_TARGET common CACHE STRING "Build target?")
message("[BUILD] ${BUILD_TARGET}")
include(${BUILD_TARGET}.cmake)

add_custom_target(build-test)

enable_testing()


file(GLOB HEADER_FILES inc/constrained_any.hpp )

add_library(yan.constrained_any INTERFACE)
target_sources(
    yan.constrained_any
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc
        FILES
            ${HEADER_FILES}
)
target_include_directories(yan.constrained_any INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)
add_library(yan::constrained_any ALIAS yan.constrained_any)

install(
    TARGETS yan.constrained_any
    EXPORT yan.constrained_any
    FILE_SET HEADERS
)

add_subdirectory(test EXCLUDE_FROM_ALL)
add_subdirectory(sample EXCLUDE_FROM_ALL)
