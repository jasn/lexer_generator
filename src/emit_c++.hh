#ifndef EMIT_CPP_HH_GUARD
#define EMIT_CPP_HH_GUARD

#include <ostream>
#include <string>
#include <vector>

#include "DFA.hh"

namespace lexer {


  void emit_dfa(const DFA &d, std::vector<std::string> &names, 
		std::ostream &hhFile, std::ostream &ccFile);

} // end namespace lexer

#endif
