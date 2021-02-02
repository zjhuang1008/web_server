file(GLOB_RECURSE net_srcs CONFIGURE_DEPENDS "*.cc")
# message("net_srcs: " ${net_srcs})
find_package(Boost REQUIRED COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

add_library(net ${net_srcs})
target_link_libraries(net pthread logger ${Boost_LIBRARIES})

# install(TARGETS net DESTINATION lib)

# file(GLOB net_headers "*.h")
# install(FILES ${net_headers} DESTINATION include)
