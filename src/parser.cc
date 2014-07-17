#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_set>

#include "parser.hh"
#include "RegularExpression.hh"


namespace lexer {

Parser::Parser(std::string line, size_t linenumber, size_t add) : 
  line(line), linenumber(linenumber), add(add), pos(this->line.data()), 
  end(this->line.data()+this->line.size()) {
  
  parseTree = parseOr();
}

std::shared_ptr<RegularExpression> Parser::parseOr() {
  std::shared_ptr<RegularExpression> left = parseConcat();
  if (*pos == '|') {
    ++pos;
    std::shared_ptr<RegularExpression> right = parseOr();
    return std::make_shared<RegExpOr>(left, right);
  }
  return left;

}

std::shared_ptr<RegularExpression> Parser::parseConcat() {
  std::shared_ptr<RegularExpression> left = parseStarPlus();
  if (pos != end && *pos != '|' && *pos != ')') {
    std::shared_ptr<RegularExpression> right = parseConcat();
    return std::make_shared<RegExpConcat>(left, right);
  }
  return left;
}

std::shared_ptr<RegularExpression> Parser::parseStarPlus() {
  std::shared_ptr<RegularExpression> left = parseInner();
  if (*pos == '*') {
    ++pos;
    return std::make_shared<RegExpStar>(left);
  } else if (*pos == '+') {
    ++pos;
    return std::make_shared<RegExpPlus>(left);
  }
  return left;
}

std::shared_ptr<RegularExpression> Parser::parseInner() {
  if (*pos == '\\' && pos+1 < end) {
    ++pos;
    return std::make_shared<RegExpChars, std::initializer_list<symbol>>({static_cast<symbol>(*pos++)}, false);
  }
  if (*pos == '.') {
    ++pos;
    return std::make_shared<RegExpChars, std::initializer_list<symbol>>({}, true);
  } 
  if (*pos == '[') {
    ++pos;
    bool negate = false;
    if (pos != end && *pos == '^') {
      negate = true;
      ++pos;
    }
  
    std::unordered_set<symbol> chars;
    while (*pos != ']') {
      switch (*pos) {
      case '\\':
	++pos;
	switch (*pos) {
	case '\0': {
	  std::stringstream error_string;
	  error_string << "Parser error: unexpected null termination" << std::endl;
	  error_string << "Error on line " << linenumber << " at position "
		       << (pos - this->line.data()) + add << std::endl;
	  throw std::runtime_error(error_string.str());
	}
	case 'n':
	  chars.insert(symbol('\n'));
	  break;
	case 'r':
	  chars.insert(symbol('\r'));
	  break;
	case 't':
	  chars.insert(symbol('\t'));
	  break;
	default:
	  chars.insert(static_cast<symbol>(*pos));
	}
	++pos;
	break;
      case '-': {
	// note [-z] means all characters from '[' to 'z'.
	// because of laziness. [-] means '[', '\', ']'
	// [!--b] is also fun.
	char l = *(pos-1);
	char r = *(pos+1);
	if (r == '\\') r = *(pos+2);
	if (l > r) {
	  throw std::runtime_error("Parser error: Range-error left greater than right");
	  exit(EXIT_FAILURE);
	}
	chars.insert(static_cast<symbol>(r));
	while (l != r) {
	  chars.insert(static_cast<symbol>(l++));
	}
	++pos;
	break;
      }
      case '\0':
	{
	  std::stringstream error_string;
	  error_string << "Parser error: unexpected null termination" << std::endl;
	  error_string << "Error on line " << linenumber << " at position "
		       << (pos - this->line.data()) + add << std::endl;
	  throw std::runtime_error(error_string.str());
	}
	break;
      default:
	chars.insert(static_cast<symbol>(*pos++));
	break;
      }
    }
    ++pos;
    return std::make_shared<RegExpChars>(chars, negate);
  }

  if (*pos == '(') {
    ++pos;
    std::shared_ptr<RegularExpression> center = parseOr();
    if (*pos != ')') {
      std::stringstream error_string;
      error_string << "Parser error: expected right parenthesis but found '";
      error_string << *pos << "'";
      throw std::runtime_error(error_string.str());
    }
    ++pos;
    return center;
  }
  if (*pos == '+' || *pos == '*' || *pos == ')' || *pos == ']' || *pos == '\\') {
    std::stringstream error_string;
    error_string << "Parser error: unexpected symbol: '" << *pos << "'" << std::endl;
    error_string << "Error on line " << linenumber << " at position "
		 << (pos - this->line.data()) + add << std::endl;
    throw std::runtime_error(error_string.str());
  }
  return std::make_shared<RegExpChars, std::initializer_list<symbol>>({static_cast<symbol>(*pos++)}, false);

}

tkn_rule parseLine(std::string &line, size_t linenumber) {

  std::stringstream ss;
  size_t pos = 0;
  for (; pos < line.length()-1; ++pos) {
    if (line[pos] == ':' && line[pos+1] == '=') {
      pos += 2;
      break;
    }

    if (line[pos] != ' ')
      ss << line[pos];
  }
  
  while (line[pos] == ' ') ++pos;

  std::string name = ss.str();
  std::string rest = line.substr(pos);
  Parser p(rest, linenumber, pos);

  tkn_rule a = {name, p.parseTree, name[0]=='_'};

  return a;

}


std::vector<tkn_rule>
parseFile(std::istream &file) {
  std::vector<tkn_rule> ret;
  std::string line;
  size_t linenumber = 0;
  while (file.good()) {
    ++linenumber;
    std::getline(file, line);
    if (line.size() == 0) continue;
    ret.push_back(parseLine(line, linenumber));
  }
  return ret;
}

}
