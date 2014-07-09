#ifndef PARSER_HH_GUARD
#define PARSER_HH_GUARD

#include <string>
#include <memory>

#include "RegularExpression.hh"

namespace lexer {

class Parser {

public:
  Parser(std::string line);
  std::shared_ptr<RegularExpression> parseOr();
  std::shared_ptr<RegularExpression> parseConcat();
  std::shared_ptr<RegularExpression> parseStarPlus();
  std::shared_ptr<RegularExpression> parseInner();

  std::shared_ptr<RegularExpression> parseTree;

private:
  std::string line;
  const char *pos;
  const char *end;
  
};

std::pair<std::string, std::shared_ptr<RegularExpression> > parseLine(std::string &line);

void parseFile(std::ifstream file);

} // end namespace lexer
#endif

