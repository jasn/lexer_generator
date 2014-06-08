#include <iostream>
#include <map>
#include <vector>

#include "../src/DFA.hh"

size_t counter = 1;

void printResult(bool success) {
  std::cout << "Test case #" << counter++ << ":\t" << (success?"Pass":"Fail") << std::endl;
}

void test1() {
  
  // Creates an automaton that accepts all binary strings that end with at least
  // two '0' characters.

  std::map<std::pair<lexer::state, lexer::symbol>, lexer::state> d;
  d.insert(std::make_pair(std::make_pair(0, '0'), 1));
  d.insert(std::make_pair(std::make_pair(0, '1'), 0));
  d.insert(std::make_pair(std::make_pair(1, '0'), 2));
  d.insert(std::make_pair(std::make_pair(1, '1'), 0));
  d.insert(std::make_pair(std::make_pair(2, '0'), 2));
  d.insert(std::make_pair(std::make_pair(2, '1'), 0));
  std::vector<lexer::state> acc = {2};

  lexer::DFA automaton(3, acc, 0, d);

  std::vector<std::string> positiveTestCases = { "01100","00","100","100100" };
  std::vector<std::string> negativeTestCases = { "0110","010","110","1001001" };

  for (size_t i = 0; i < positiveTestCases.size(); ++i) {
    printResult(automaton.accept(positiveTestCases[i]));
  }

  for (size_t i = 0; i < positiveTestCases.size(); ++i) {
    printResult(!automaton.accept(negativeTestCases[i]));
  }

}


int main() {

  test1();

}
