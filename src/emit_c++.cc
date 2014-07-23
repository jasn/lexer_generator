#include "emit_c++.hh"
#include "DFA.hh"
#include "parser.hh"

#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include "time.h"

using namespace lexer;

const std::string indent = "    ";

void lexer::cpp_emitter::emit_dfa(std::vector<tkn_rule> &tkn_rules,
				  const std::string &outputDirectory) {

  std::string hhFilename = outputDirectory + "tokenizer.hh";
  std::string ccFilename = outputDirectory + "tokenizer.cc";

  std::ofstream hhFile(hhFilename);
  std::ofstream ccFile(ccFilename);

  if (hhFile.fail()) {
    throw std::runtime_error("Could not open: " + hhFilename);
  }
  if (ccFile.fail()) {
    throw std::runtime_error("Could not open: " + ccFilename);
  }

  std::vector<std::string> names(tkn_rules.size());
  NFA f(1, std::unordered_map<state, acceptType>(), 0, {});
  for (size_t i = 0; i < tkn_rules.size(); ++i) {
    names[i] = tkn_rules[i].name;
    f = lexer::NFA::join(f, tkn_rules[i].regexp->getNFA(i+1));
  }

  DFA d = f.determinize();
  d.minimize();

  std::ofstream ff("monkey.dot");
  ff << d.toDot();

  std::stringstream oscc;
  std::stringstream oshh;
  hhFile << "#ifndef TOKENIZER_HH_GUARD" << std::endl;
  hhFile << "#define TOKENIZER_HH_GUARD" << std::endl << std::endl;

  hhFile << "#include <iostream>" << std::endl;
  hhFile << "#include <stdint.h>" << std::endl << std::endl;

  hhFile << "namespace lexer {" << std::endl << std::endl;

  hhFile << "enum class TokenType {" << std::endl;
  hhFile << indent;
  for (auto &s : names) {
    hhFile << s << ", ";
  }
  hhFile << "END_OF_FILE, INVALID" << std::endl << std::endl;

  hhFile << "};" << std::endl << std::endl;

  hhFile << "std::ostream& operator<<(std::ostream &os, const lexer::TokenType &t);" << std::endl << std::endl;

  hhFile << "struct Token {" << std::endl;
  hhFile << indent << "const char *start, *curr;" << std::endl;
  hhFile << indent << "TokenType tkn;" << std::endl;
  hhFile << "};" << std::endl;

  hhFile << std::endl << std::endl;

  hhFile << "struct Tokenizer {" << std::endl << std::endl;
  hhFile << indent << "const char *str;" << std::endl << std::endl;
  hhFile << indent << "Tokenizer(const char *str) : str(str) {}" << std::endl << std::endl;
  hhFile << indent << "Token getNextToken();" << std::endl << std::endl;
  hhFile << "};" << std::endl << std::endl;
  
  hhFile << "} // end namespace lexer" << std::endl << std::endl;

  hhFile << "#endif // TOKENIZER_HH_GUARD" << std::endl;
  
  ccFile << "#include \"tokenizer.hh\"" << std::endl << std::endl;
  
  ccFile << "namespace lexer {" << std::endl << std::endl;

  ccFile << "std::ostream& operator<<(std::ostream &os, const lexer::TokenType &t) {" << std::endl;
  ccFile << indent << "switch (t) {" << std::endl;
  for (auto &s : names) {
    ccFile << indent << "case lexer::TokenType::" << s << ":" << std::endl;
    ccFile << indent << indent << "os << \"" << s << "\";" << std::endl; 
    ccFile << indent << indent << "break;" << std::endl;
  }
  ccFile << indent << "case lexer::TokenType::END_OF_FILE:" << std::endl;
  ccFile << indent << indent << "os << \"END_OF_FILE\";" << std::endl; 
  ccFile << indent << indent << "break;" << std::endl;
  ccFile << indent << "case lexer::TokenType::INVALID:" << std::endl;
  ccFile << indent << "default:" << std::endl;
  ccFile << indent << indent << "os << \"INVALID\";" << std::endl;
  ccFile << indent << "}" << std::endl << std::endl;
  ccFile << indent << "return os;" << std::endl;
  ccFile << "}" << std::endl << std::endl;

  ccFile << "Token Tokenizer::getNextToken() {" << std::endl << std::endl;
  
  ccFile << std::endl;
  ccFile << indent << "const uint8_t *start;" << std::endl;
  ccFile << indent << "const uint8_t *curr = reinterpret_cast<const uint8_t*>(str);" << std::endl;
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
      ccFile << indent << indent << "str = reinterpret_cast<const char*>(curr);" << std::endl;
      ccFile << indent << indent 
	     << "return Token{reinterpret_cast<const char*>(start), reinterpret_cast<const char*>(curr), TokenType::END_OF_FILE};" << std::endl;
    }
    
    acceptType a = d.getAcceptTypeForState(x.first, lexer::REJECT);

    ccFile << indent << "default: " << std::endl;
    if (a == lexer::REJECT) {
      if (x.first == d.getInitialState()) {
	ccFile << indent << indent << "++curr;" << std::endl;
      }
      ccFile << indent << indent << "str = reinterpret_cast<const char*>(curr);" << std::endl;
      ccFile << indent << indent << "return Token{reinterpret_cast<const char*>(start), reinterpret_cast<const char*>(curr), TokenType::INVALID};" << std::endl;
    } else if (names[a-1][0] == '_') { // ignore => go back to start.
      ccFile << indent << indent << "goto beginning;" << std::endl;
    } else {
      ccFile << indent << indent << "str = reinterpret_cast<const char*>(curr);" << std::endl;
      ccFile << indent << indent << "return Token{reinterpret_cast<const char*>(start), reinterpret_cast<const char*>(curr), TokenType::" << names[a-1] << "};" << std::endl;
    }

    ccFile << indent << "}" << std::endl;
  }

  ccFile << std::endl << "}" << std::endl << std::endl;

  ccFile << "} // end namespace lexer" << std::endl;
}
