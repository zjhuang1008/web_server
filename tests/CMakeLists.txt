
add_executable(test_event_loop test_event_loop.cc)
target_link_libraries(test_event_loop net)

add_executable(test_server test_server.cc)
target_link_libraries(test_server net)

add_executable(test_linked_buffer test_linked_buffer.cc)
target_link_libraries(test_linked_buffer net)

add_executable(test_http_req_hash test_http_req_hash.cc)
target_link_libraries(test_http_req_hash net)

add_executable(webbench ./webbench/webbench.c)