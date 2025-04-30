cmake_minimum_required(VERSION 3.16)

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)



add_executable(test_constrained_any EXCLUDE_FROM_ALL test_src/test_constrained_any.cpp)
target_include_directories( test_constrained_any  PRIVATE ../inc )
target_link_libraries(test_constrained_any gtest gtest_main )
add_dependencies(build-test test_constrained_any)
add_test(NAME test_constrained_any COMMAND $<TARGET_FILE:test_constrained_any>)


