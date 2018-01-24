cmake_minimum_required(VERSION 3.1)

project(Corecat)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_WARNINGS /EHsc /W3")
endif()

include_directories("include")

set(EXAMPLE
    Any
    CommandLine
    ExceptionWrapper
    Range
    String
    System
    X86Feature)

foreach(example ${EXAMPLE})
    add_executable(${example} example/${example}/${example}.cpp)
endforeach()
