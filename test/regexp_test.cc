#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include "../src/emit_c++.hh"
#include "../src/RegularExpression.hh"
#include "../src/parser.hh"

using namespace lexer;

int main(int argc, char *argv[]) {

  std::stringstream small_lang(
      "LITERAL := [_a-zA-Z][_a-zA-Z0-9]*\n"
      "NUMBER := 0|[1-9][0-9]*\n"
      "IF := if\n"
      "ELSE := else\n"
      "WHILE := while\n"
      "_COMMENT := (//|#)[^\\n]*\n"
      "_WHITESPACE := [\\t ]+\n"
      "_NEWLINE := ([\\n]+)|(([\\r][\\n])+)\n"
			       );

  std::vector<tkn_rule> thingy = parseFile(small_lang);
      
  std::string outputDirectory = std::string(CMAKE_SOURCE_DIR)+"/build/";

  NFA f(1, std::unordered_map<state, acceptType>(), 0, {});
  for (size_t i = 0; i < thingy.size(); ++i)
    f = lexer::NFA::join(f, thingy[i].regexp->getNFA(i+1));
  DFA d = f.determinize();
  d.minimize();

  cpp_emitter::emit_dfa(d, thingy, outputDirectory);
}
