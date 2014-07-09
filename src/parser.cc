#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_set>

#include "parser.hh"
#include "RegularExpression.hh"


namespace lexer {

  Parser::Parser(std::string line) : line(line), pos(this->line.data()), end(this->line.data()+this->line.size()) {
  
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
    std::unordered_set<char> tmp({*pos++});
    return std::make_shared<RegExpChars, std::initializer_list<char>>({*pos++}, false);
  }
  if (*pos == '.') {
    ++pos;
    return std::make_shared<RegExpChars, std::initializer_list<char>>({}, true);
  } 
  if (*pos == '[') {
    ++pos;
    bool negate = false;
    if (pos != end && *pos == '^') {
      negate = true;
      ++pos;
    }
  
    std::unordered_set<char> chars;
    while (*pos != ']') {
      switch (*pos) {
      case '\\':
	++pos;
	if (pos == end) {
	  std::cerr << "you failed" << std::endl;
	  exit(EXIT_FAILURE);	  
	}
	chars.insert(*pos++);
	break;
      case '-': {
	// note [-z] means all characters from '[' to 'z'.
	// because of laziness. [-] means '[', '\', ']'
	// [!--b] is also fun.
	char l = *(pos-1);
	char r = *(pos+1);
	if (r == '\\') r = *(pos+2);
	if (l > r) {
	  std::cerr << "you failed" << std::endl;
	  exit(EXIT_FAILURE);
	}
	chars.insert(r);
	while (l != r) {
	  chars.insert(l++);
	}
	++pos;
	break;
      }
      case '\0':
	std::cerr << "you failed" << std::endl;
	exit(EXIT_FAILURE);
	break;
      default:
	chars.insert(*pos++);
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
      std::cerr << "you failed" << std::endl;
      exit(EXIT_FAILURE);
    }
    ++pos;
    return center;
  }
  if (*pos == '+' || *pos == '*' || *pos == ')' || *pos == ']' || *pos == '\\') {
    std::cerr << "you failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  return std::make_shared<RegExpChars, std::initializer_list<char>>({*pos++}, false);

}

std::pair<std::string, std::shared_ptr<RegularExpression> > parseLine(std::string &line) {

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
  
  Parser p(rest);

  return make_pair(name, p.parseTree);

}

void parseFile(std::ifstream file) {

  std::string line;
  while (file.good()) {
    std::getline(file, line);
    parseLine(line);
  }

}

}
