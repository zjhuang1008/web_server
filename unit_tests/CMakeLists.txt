cmake_minimum_required(VERSION 3.0.0)
project(web_server_unit_test C CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(srcs_path ${PROJECT_SOURCE_DIR}/../srcs)
message(STATUS "srcs_path: " ${srcs_path})

enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/..)
add_library(fd_handler ${srcs_path}/net/fd_handler/fd_handler.cc ${srcs_path}/net/fd_handler/fd_handler.h)
add_executable(test_fd_handler test_fd_handler.cc)
target_link_libraries(test_fd_handler fd_handler)
