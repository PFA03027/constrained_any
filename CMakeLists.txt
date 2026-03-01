cmake_minimum_required(VERSION 3.16)
project(
    constrained_any
    #
    DESCRIPTION "std::any is very useful. But it is too free. yan::constrained_any introduces the constrain. Then, e.g., yan::constrained_any can be used as key value."
    LANGUAGES CXX
    VERSION 1.6.1
    )

set(CMAKE_VERBOSE_MAKEFILE 1)


# Switch build target
set(BUILD_TARGET common CACHE STRING "Build target?")
message("[BUILD] ${BUILD_TARGET}")
include(${BUILD_TARGET}.cmake)

add_custom_target(build-test)

enable_testing()


add_subdirectory(constrained_any)
add_subdirectory(test EXCLUDE_FROM_ALL)
add_subdirectory(sample EXCLUDE_FROM_ALL)
