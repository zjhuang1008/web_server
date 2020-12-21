file(GLOB_RECURSE net_srcs CONFIGURE_DEPENDS "*.cc")
message("net_srcs: " ${net_srcs})

add_library(net ${net_srcs})
target_link_libraries(net pthread logger)

# install(TARGETS net DESTINATION lib)

# file(GLOB net_headers "*.h")
# install(FILES ${net_headers} DESTINATION include)
