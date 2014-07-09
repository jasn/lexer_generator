add_executable(DFA_test DFA_test.cc)
add_executable(NFA_test NFA_test.cc)
add_executable(parser_test parser_test.cc)

target_link_libraries(DFA_test lexer)
target_link_libraries(NFA_test lexer)
target_link_libraries(parser_test lexer)
