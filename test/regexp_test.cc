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

  // std::string x = "X := ab*aa(bba)*ab|aab[ab]";

  // tkn_rule parsed(lexer::parseLine(x));

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
  std::vector<std::string> names(thingy.size());
  NFA f(1, std::unordered_map<state, acceptType>(), 0, {});
  for (size_t i = 0; i < thingy.size(); ++i) {
    names[i] = thingy[i].name;
    //thingy[i].regexp->printType(std::cout);
    //std::cout << thingy[i].regexp->getNFA(i+1).toDot() << std::endl;
    f = lexer::NFA::join(f, thingy[i].regexp->getNFA(i+1));
    //std::cout << f.toDot() << std::endl;
  }
      
  //DFA fine = thingy[5].regexp->getNFA(1).determinize();
  //fine.minimize();
  //std::cout << fine.toDot() << std::endl;

  DFA d = f.determinize();
  d.minimize();
  //std::cout << d.toDot() << std::endl;
      
  //std::cout << std::endl << std::endl;

  //std::cout << emit_dfa(d, names) << std::endl;
  
  emit_dfa(d, names, std::cout, std::cout);
}
