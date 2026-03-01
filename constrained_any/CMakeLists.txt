cmake_minimum_required(VERSION 3.16)

file(GLOB HEADER_FILES inc/*.hpp )

add_library(yan.constrained_any INTERFACE)
# target_sources(
#     constrained_any
#     INTERFACE
#         FILE_SET HEADERS
#         BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc
#         FILES
#             ${HEADER_FILES}
# )
set_target_properties(yan.constrained_any PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
target_include_directories(yan.constrained_any INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)
add_library(yan::constrained_any ALIAS yan.constrained_any)

# install(
#     TARGETS constrained_any
#     EXPORT constrained_any
#     FILE_SET HEADERS
# )
include(GNUInstallDirs)
install(TARGETS yan.constrained_any
        PUBLIC_HEADER
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
)
