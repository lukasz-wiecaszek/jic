cmake_minimum_required(VERSION 3.14) # it's just a nice number

project(stream VERSION 0.0.1)

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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
         "Choose the type of build. Options are: {Release, Debug}." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

find_package(GTest)
include(GoogleTest)
enable_testing()

set(UNITTEST_FILES
    istream.hpp
    ostream.hpp
)

add_compile_options(-Werror -Wall -Wextra -Wno-maybe-uninitialized -pedantic)
add_compile_options(-fno-exceptions -fprofile-arcs -ftest-coverage)

add_executable(stream_test
    istream_be_test.cpp
    istream_le_test.cpp
    ostream_be_test.cpp
    ostream_le_test.cpp
)

target_include_directories(stream_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../utils
)

target_link_libraries(stream_test
    PUBLIC
        gcov
        GTest::gtest_main
)

gtest_discover_tests(stream_test)

set(WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/stream_test.dir/)

add_custom_target(lcov
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/lcov.d
    COMMAND lcov --ignore-errors inconsistent,inconsistent,
                 --base-directory ${CMAKE_CURRENT_SOURCE_DIR}
                 --directory ${WORKING_DIR}
                 --initial --capture -output lcov.d/coverage.init
    COMMAND ${CMAKE_CTEST_COMMAND}
)

add_custom_command(TARGET lcov POST_BUILD
    COMMAND lcov --ignore-errors inconsistent,inconsistent
                 --base-directory ${CMAKE_CURRENT_SOURCE_DIR}
                 --directory ${WORKING_DIR}
                 --initial --capture -output lcov.d/coverage.run
    COMMAND lcov --add-tracefile lcov.d/coverage.init
                 --add-tracefile lcov.d/coverage.run
                 --output lcov.d/coverage.total
    COMMAND lcov --extract lcov.d/coverage.total
                 ${CMAKE_CURRENT_SOURCE_DIR}/${UNITTEST_FILES}
                 --output lcov.d/coverage.info
    COMMAND genhtml lcov.d/coverage.info --output-directory lcov.d
    COMMAND rm -f ${WORKING_DIR}/*.gcda
)

add_dependencies(lcov stream_test)