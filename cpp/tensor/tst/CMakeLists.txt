cmake_minimum_required(VERSION 3.3)

project(tensor_tests VERSION 1.0.0)

# sets various paths used in e.g. pc.in files as well as install target
include(GNUInstallDirs)

message(STATUS "Processing CMakeLists.txt for: " ${PROJECT_NAME} " " ${PROJECT_VERSION})

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
# this is the top level directory of your build tree
message(STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR})

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to
message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})

# this is the directory, from which cmake was started, i.e. the top level source directory
message(STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR})

# this is the directory where the currently processed CMakeLists.txt is located in
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
         "Choose the type of build. Options are: {Release, Debug}." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} --coverage)

set(CFGITEMS_TESTS_API_DIR
)

set(CFGITEMS_TESTS_INC_DIR
)

set(CFGITEMS_TESTS_SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
add_subdirectory(googletest)

function(add_test_executable name)
    add_executable(${name} ${name})
    target_link_libraries(${name} PRIVATE gtest_main tensor)
endfunction()

add_test_executable(tensor_tests)
add_test(NAME test01 COMMAND $<TARGET_FILE:tensor_tests>)

add_test_executable(dimensions_tests)
add_test(NAME test02 COMMAND $<TARGET_FILE:dimensions_tests>)
