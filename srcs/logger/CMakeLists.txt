file(GLOB_RECURSE logger_srcs CONFIGURE_DEPENDS "*.cc")
# message(STATUS "logger srcs: " ${logger_srcs})
add_library(logger ${logger_srcs})
target_link_libraries(logger timer)
