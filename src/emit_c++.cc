#include "emit_c++.hh"
#include "DFA.hh"

#include <map>
#include <set>
#include <iostream>
#include <sstream>

using namespace lexer;

std::string lexer::emit_dfa(const DFA &d, std::vector<std::string> &names) {
  std::stringstream os;

  os << "enum class TokenType {" << std::endl;
  for (auto &s : names) {
    os << s << ", ";
  }
  os << "END_OF_FILE, INVALID" << std::endl << "};" << std::endl << std::endl;

  os << "struct Token {" << std::endl;
  os << "char *start, *curr;" << std::endl;
  os << "TokenType tkn" << std::endl;
  os << "};" << std::endl;

  os << std::endl << std::endl;
  os << "Token getNextToken(char *curr) {" << std::endl << std::endl;

  auto delta = d.getDelta();
  auto accepts = d.getAcceptStates();
  size_t numberOfStates = d.getNumberOfStates();
  
  state rejectState;
  for (state s = 0; s < numberOfStates; ++s) {
    auto iter = delta.lower_bound({s, symbol::min() });
    if (accepts.count(s)) continue;
    while (iter->first.first == s) {
      if (iter->second != s) break;
      ++iter;
    }
    if (iter->first.first == s) continue;
    rejectState = s;
    break;
  }

  std::map<state, std::map<state, std::set<symbol> > > remapped;

  for (auto x : delta) {
    if (x.second == rejectState) {
      continue;
    }
    if (x.first.second == symbol('\0')) {
      continue;
    }
    remapped[x.first.first][x.second].insert(x.first.second);
  }


  for (auto x : remapped) {
    os << "s" << x.first << ":" << std::endl;
    os << "switch (*curr) {" << std::endl;
    for (auto y : x.second) {
      for (auto z : y.second) {
	os << "case " << static_cast<int>(z.val) << ":";// << std::endl;
      }
      os << "++curr;" << std::endl;
      os << "goto s" << y.first << ";" << std::endl;
    }

    if (x.first == d.getInitialState()) {
      os << "case 0: return {start, curr, Token.END_OF_FILE};" << std::endl;
    }
    
    acceptType a = d.getAcceptTypeForState(x.first, lexer::REJECT);

    os << "default: " << std::endl;
    if (a == lexer::REJECT) {
      os << "return {start, curr, Token.INVALID};" << std::endl;
    } else if (names[a-1][0] == '_') { // ignore => go back to start.
      os << "goto start;" << std::endl;
    } else {
      os << "return {start, curr, Token." << names[a-1] << "};" << std::endl;
    }

    os << "}" << std::endl;
  }

  os << std::endl << "}" << std::endl;

  
  return os.str();
}
