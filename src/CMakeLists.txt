add_library (lexer
	    DFA.hh
	    DFA.cc
	    NFA.hh
	    NFA.cc
	    lexer_common.hh
	    parser.hh
	    parser.cc
	    RegularExpression.hh
	    emit_c++.hh
	    emit_c++.cc
)

add_executable(generate_lexer generate_lexer.cc)
target_link_libraries(generate_lexer lexer)
