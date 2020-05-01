cmake_minimum_required(VERSION 3.0)
project (noname_tools)

enable_testing()

if(NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED on)

if(${CMAKE_CXX_STANDARD} STREQUAL "14")
    add_compile_definitions(NONAME_CPP14)
elseif(${CMAKE_CXX_STANDARD} STREQUAL "17")
    add_compile_definitions(NONAME_CPP17)
elseif(${CMAKE_CXX_STANDARD} STREQUAL "20")
    add_compile_definitions(NONAME_CPP17)
else()
    message(SEND_ERROR "Unsupported CMAKE_CXX_STANDARD value of ${CMAKE_CXX_STANDARD} supplied. Only 14, 17 and 20 are supported.")
endif()

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
