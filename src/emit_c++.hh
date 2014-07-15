#ifndef EMIT_CPP_HH_GUARD
#define EMIT_CPP_HH_GUARD

#include <string>
#include <vector>

#include "DFA.hh"

namespace lexer {


  std::string emit_dfa(const DFA &d, std::vector<std::string> &names);


} // end namespace lexer

#endif
