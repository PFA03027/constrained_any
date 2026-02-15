cmake_minimum_required(VERSION 3.16)

# add_custom_target(build-test)

add_executable(sample_of_constrained_any EXCLUDE_FROM_ALL sample_of_constrained_any.cpp)
# target_include_directories( sample_of_constrained_any  PRIVATE ../inc )
target_compile_options(sample_of_constrained_any PUBLIC  -Wall -Wconversion -Wsign-conversion -Werror)
target_link_libraries(sample_of_constrained_any yan::constrained_any )
add_dependencies(build-test sample_of_constrained_any)


