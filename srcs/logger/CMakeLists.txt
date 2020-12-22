file(GLOB_RECURSE logger_srcs CONFIGURE_DEPENDS "*.cc")
add_library(logger ${logger_srcs})
target_link_libraries(logger timer)
