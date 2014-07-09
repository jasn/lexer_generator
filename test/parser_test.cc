#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../src/parser.hh"
#include "../src/RegularExpression.hh"

#define KEGLE "40"

using namespace lexer;

std::stringstream ss;

void test_concat() {

  std::string line = "X := abc";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);

}

void test_star() {
  std::string line = "X := a*";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);
}

void test_plus() {
  std::string line = "X := a+";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);
}

void test_or() {
  std::string line = "X := a|b";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);

}

void test_range() {
  std::string line = "X := [a-z]";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);
}

void test_range_negated() {
  std::string line = "X := [^a-z]";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);
}

void test_parenthesis() {
  std::string line = "X := (aab)*a";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);

}

void test_precedence() {
  std::string line = "X := ab*aa(bba)*ab|aab[ab]";

  std::pair<std::string, std::shared_ptr<RegularExpression> > parsed(lexer::parseLine(line));

  parsed.second->printType(ss);
}

int main() {

  ss << "{\"test_concat\":" << std::endl;
  test_concat();

  ss << ", \"test_star\":" << std::endl;
  test_star();

  ss << ", \"test_plus\":" << std::endl;
  test_plus();

  ss << ", \"test_or\":" << std::endl;
  test_or();

  ss << ", \"test_range\":" << std::endl;
  test_range();
  
  ss << ", \"test_range_negated\":" << std::endl;
  test_range_negated();

  ss << ", \"test_parenthesis\":" << std::endl;
  test_parenthesis();
  
  ss << ", \"test_precedence\":" << std::endl;
  test_precedence();

  ss << "}" << std::endl;

  std::ifstream ifs(std::string(CMAKE_SOURCE_DIR)+"/test/parser_test_output");
  std::stringstream ss2;
  std::string s1, s2;

  if (!ifs.good()) {
    std::cout << "Error in parser test" << std::endl;
    exit(EXIT_FAILURE);  
  }

  while (ifs.good() && ss.good()) {
    getline(ifs, s1);
    getline(ss, s2);
    if (s1 != s2) {
      std::cout << "Error in parser test" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  std::cout << "Parser tests: PASSED" << std::endl;

}
