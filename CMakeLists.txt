cmake_minimum_required(VERSION 2.8.7)
project (noname_tools)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories ("${CMAKE_CURRENT_LIST_DIR}/src")
include_directories ("${CMAKE_CURRENT_LIST_DIR}/submodules/Catch/single_include")

file(GLOB NONAME_SOURCES "${CMAKE_CURRENT_LIST_DIR}/test_tool/main.cpp")
file(GLOB NONAME_SOURCES "${CMAKE_CURRENT_LIST_DIR}/test_tool/*.cpp")
file(GLOB NONAME_HEADERS "${CMAKE_CURRENT_LIST_DIR}/src/noname_tools/*.h")

add_executable(test_tool ${NONAME_SOURCES} ${NONAME_HEADERS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT test_tool)
add_test(
    NAME test_noname_tools 
    COMMAND test_tool
)
