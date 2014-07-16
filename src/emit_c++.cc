#include "emit_c++.hh"
#include "DFA.hh"

#include <cctype>
#include <iostream>
#include <map>
#include <set>
#include <sstream>

using namespace lexer;

const std::string indent = "    ";

void lexer::emit_dfa(const DFA &d, std::vector<std::string> &names, std::ostream &hhFile, std::ostream &ccFile) {
  
  std::stringstream oscc;
  std::stringstream oshh;
  hhFile << "#ifndef TOKENIZER_HH_GUARD" << std::endl;
  hhFile << "#define TOKENIZER_HH_GUARD" << std::endl << std::endl;

  hhFile << "#include <stdint.h>" << std::endl << std::endl;
  hhFile << "enum class TokenType {" << std::endl;
  hhFile << indent;
  for (auto &s : names) {
    hhFile << s << ", ";
  }
  hhFile << "END_OF_FILE, INVALID" << std::endl << "};" << std::endl << std::endl;

  hhFile << "struct Token {" << std::endl;
  hhFile << indent << "char *start, *curr;" << std::endl;
  hhFile << indent << "TokenType tkn;" << std::endl;
  hhFile << "};" << std::endl;

  hhFile << std::endl << std::endl;

  hhFile << "struct Tokenizer {" << std::endl << std::endl;
  hhFile << indent << "char *str;" << std::endl << std::endl;
  hhFile << indent << "Tokenizer(char *str) : str(str) {}" << std::endl << std::endl;
  hhFile << indent << "Token getNextToken();" << std::endl << std::endl;
  hhFile << "};" << std::endl << std::endl;
  
  hhFile << "#endif // TOKENIZER_HH_GUARD" << std::endl;
  
  ccFile << "#include \"tokenizer.hh\"" << std::endl << std::endl;
  ccFile << "Token Tokenizer::getNextToken() {" << std::endl << std::endl;
  
  ccFile << std::endl;
  ccFile << indent << "uint8_t *start;" << std::endl;
  ccFile << indent << "uint8_t *curr = reinterpret_cast<uint8_t*>(str);" << std::endl;
  ccFile << "beginning:" << std::endl;
  ccFile << indent << "start = curr;" << std::endl << std::endl;

  auto delta = d.getDelta();
  auto accepts = d.getAcceptStates();
  size_t numberOfStates = d.getNumberOfStates();
  state q0 = d.getInitialState();

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

  ccFile << indent << "goto s" << q0 << ";" << std::endl << std::endl;

  std::map<state, std::map<state, std::set<symbol> > > remapped;

  for (state i = 0; i < numberOfStates; ++i) {
    remapped[i];
  }

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
    ccFile << "s" << x.first << ":" << std::endl;
    ccFile << indent << "switch (*curr) {" << std::endl;
    for (auto y : x.second) {
      if (x.first == rejectState) continue;
      for (auto z : y.second) {
	ccFile << indent << "case " << static_cast<int>(z.val) << ":" << std::endl;
      }
      ccFile << indent << indent << "++curr;" << std::endl;
      ccFile << indent << indent << "goto s" << y.first << ";" << std::endl;
    }

    if (x.first == d.getInitialState()) {
      ccFile << indent << "case 0:" << std::endl;
      ccFile << indent << indent << "str = reinterpret_cast<char*>(curr);" << std::endl;
      ccFile << indent << indent 
	     << "return Token{reinterpret_cast<char*>(start), reinterpret_cast<char*>(curr), TokenType::END_OF_FILE};" << std::endl;
    }
    
    acceptType a = d.getAcceptTypeForState(x.first, lexer::REJECT);

    ccFile << indent << "default: " << std::endl;
    if (a == lexer::REJECT) {
      if (x.first == d.getInitialState()) {
	ccFile << indent << indent << "++curr;" << std::endl;
      }
      ccFile << indent << indent << "str = reinterpret_cast<char*>(curr);" << std::endl;
      ccFile << indent << indent << "return Token{reinterpret_cast<char*>(start), reinterpret_cast<char*>(curr), TokenType::INVALID};" << std::endl;
    } else if (names[a-1][0] == '_') { // ignore => go back to start.
      ccFile << indent << indent << "goto beginning;" << std::endl;
    } else {
      ccFile << indent << indent << "str = reinterpret_cast<char*>(curr);" << std::endl;
      ccFile << indent << indent << "return Token{reinterpret_cast<char*>(start), reinterpret_cast<char*>(curr), TokenType::" << names[a-1] << "};" << std::endl;
    }

    ccFile << indent << "}" << std::endl;
  }

  ccFile << std::endl << "}" << std::endl;

}