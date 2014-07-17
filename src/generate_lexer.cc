#include "emit_c++.hh"
#include "parser.hh"

#include <iostream>
#include <fstream>

using namespace lexer;

void printUsage() {
  return;
}

// templated on emitter
template<typename E>
void emit(const std::string &tokenFile, const std::string &outputDirectory) {

  std::fstream fs(tokenFile);
  std::vector<tkn_rule> tkn_rules = std::move(parseFile(fs));

  E::emit_dfa(tkn_rules, outputDirectory);
  
}

int main(int argc, char *argv[]) {
  
  if (argc != 3) {
    printUsage();
  }

  std::string tokenFile(argv[1]);
  std::string outputDirectory(argv[2]);
  
  emit<cpp_emitter>(tokenFile, outputDirectory);


}
