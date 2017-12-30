cmake_minimum_required(VERSION 3.1)

project(Corecat)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -Ofast")
endif()

include_directories("include")

set(EXAMPLE
    Any
    CommandLine
    ExceptionWrapper
    String
    System
    X86Feature)

foreach(example ${EXAMPLE})
    add_executable(${example} example/${example}/${example}.cpp)
endforeach()
