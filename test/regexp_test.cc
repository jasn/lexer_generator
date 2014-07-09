#include <iostream>
#include <string>

#include "../src/RegularExpression.hh"
#include "../src/parser.hh"

using namespace lexer;

int main(int argc, char *argv[]) {

  std::string x = "X := ab*aa(bba)*ab|aab[ab]";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(x));

  NFA f = parsed.second->getNFA(1);
  DFA d = f.determinize();
  d.minimize();
  std::cout << d.toDot() << std::endl;
  std::string input(argv[1]);

  if (f.accept(input)) {
    std::cout << "juuhuu" << std::endl;
  } else {
    std::cout << "no accept. you failed." << std::endl;
  }

}
