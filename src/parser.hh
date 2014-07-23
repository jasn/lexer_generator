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
  Parser(std::string line, size_t linenumber, size_t add);
  std::shared_ptr<RegularExpression> parseOr();
  std::shared_ptr<RegularExpression> parseOpt();
  std::shared_ptr<RegularExpression> parseConcat();
  std::shared_ptr<RegularExpression> parseStarPlus();
  std::shared_ptr<RegularExpression> parseInner();

  std::shared_ptr<RegularExpression> parseTree;

private:
  std::string line;
  size_t linenumber;
  size_t add;
  
  const char *pos;
  const char *end;
};

tkn_rule parseLine(std::string &line, size_t linenumber);

std::vector<tkn_rule> parseFile(std::istream &file);

} // end namespace lexer
#endif

