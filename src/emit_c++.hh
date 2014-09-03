#ifndef EMIT_CPP_HH_GUARD
#define EMIT_CPP_HH_GUARD

#include <ostream>
#include <string>
#include <vector>

#include "DFA.hh"
#include "parser.hh"

namespace lexer {

  struct cpp_emitter {
    static void emit_dfa(const DFA & d, 
			 std::vector<tkn_rule> &tkn_rules,
			 const std::string &outputDirectory);

  };

} // end namespace lexer

#endif
