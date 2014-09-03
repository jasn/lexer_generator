#ifndef EMIT_TABLE_HH_GUARD
#define EMIT_TABLE_HH_GUARD

#include <ostream>
#include <string>
#include <vector>

#include "DFA.hh"
#include "parser.hh"

namespace lexer {

  struct table_emitter {
    static void emit_dfa(const DFA & d,
			 std::vector<tkn_rule> &tkn_rules,
			 const std::string &outputDirectory);

  };

} // end namespace lexer

#endif
