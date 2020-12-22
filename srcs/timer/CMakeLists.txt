file(GLOB_RECURSE timer_srcs CONFIGURE_DEPENDS, "*.cc")
add_library(timer ${timer_srcs})
