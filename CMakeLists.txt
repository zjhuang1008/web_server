cmake_minimum_required(VERSION 3.0.0)
project(web_server C CXX)

option(BUILD_TESTS "whether to build tests" ON)

option(WITH_EPOLL "set ON to use EPOLL, set OFF to use POLL" ON)
if (${WITH_EPOLL})
  message(STATUS "build with epoll")
  add_definitions(-DUSE_EPOLL)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

include(CTest)
enable_testing()

set(CXX_FLAGS 
    -g
    -std=c++11
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
)

add_compile_options(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})
# message("project_source_dir: " ${PROJECT_SOURCE_DIR})

add_subdirectory(srcs)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

if (${BUILD_TESTS})
  message(STATUS "build tests")
  add_subdirectory(tests)
endif()
