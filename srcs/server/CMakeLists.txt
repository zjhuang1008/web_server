file(GLOB_RECURSE server_srcs CONFIGURE_DEPENDS "*.cc")

add_library(server ${server_srcs})
target_link_libraries(server net)
