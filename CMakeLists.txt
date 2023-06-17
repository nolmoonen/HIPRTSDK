cmake_minimum_required(VERSION 3.12)

project(hiprtsdk)

function(ADD_TUTORIAL NAME)
    set(PATH "${CMAKE_SOURCE_DIR}/tutorials/${NAME}")
    file(GLOB_RECURSE PATH_SOURCES CONFIGURE_DEPENDS "${PATH}/*.h" "${PATH}/*.cpp")
    add_executable(${NAME} ${PATH_SOURCES})
    target_link_libraries(${NAME} PRIVATE tutorial_common)
    # tutorial code looks for '../common/TutorialKernels.h'
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    add_custom_command(TARGET ${NAME} COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/tutorials/common" "${CMAKE_BINARY_DIR}/common")
    # tutorial code looks for '../hiprt/hiprt.h' or '../../hiprt/hiprt.h'
    # todo only copy header files
    add_custom_command(TARGET ${NAME} COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/hiprt" "${CMAKE_BINARY_DIR}/hiprt")
endfunction()

add_library(cuew "contrib/Orochi/contrib/cuew/src/cuew.cpp")
target_include_directories(cuew PUBLIC "contrib/Orochi")

add_library(hipew "contrib/Orochi/contrib/hipew/src/hipew.cpp")
target_include_directories(hipew PUBLIC "contrib/Orochi")

add_library(orochi "contrib/Orochi/Orochi/Orochi.cpp" "contrib/Orochi/Orochi/OrochiUtils.cpp")
target_include_directories(orochi PUBLIC "contrib/Orochi")
target_link_libraries(orochi PRIVATE cuew hipew)

add_library(hiprt SHARED IMPORTED)
target_include_directories(hiprt INTERFACE "hiprt")
set_target_properties(hiprt PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/hiprt/linux64/libhiprt0200064.so")

add_library(tutorial_common STATIC "tutorials/common/TutorialBase.cpp")
target_include_directories(tutorial_common PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(tutorial_common PUBLIC orochi hiprt)

ADD_TUTORIAL("00_context_creation")
ADD_TUTORIAL("01_geom_intersection")
