
add_executable(test_event_loop test_event_loop.cc)
target_link_libraries(test_event_loop net)

add_executable(test_server test_server.cc)
target_link_libraries(test_server net)

add_executable(webbench ./webbench/webbench.c ./webbench/socket.c)