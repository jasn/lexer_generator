#ifndef PARSER_HH_GUARD
#define PARSER_HH_GUARD

#include <string>
#include <memory>
#include "RegularExpression.hh"

namespace lexer {

struct tkn_rule {
  std::string name;
  std::shared_ptr<RegularExpression> regexp;
  bool ignore = false;   

  tkn_rule(std::string name, std::shared_ptr<RegularExpression> regexp, bool ignore = false) :
    name(name), regexp(regexp), ignore(ignore) {}
};

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

tkn_rule parseLine(std::string &line);

std::vector<tkn_rule> parseFile(std::istream &file);

} // end namespace lexer
#endif

