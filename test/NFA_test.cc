#include <iostream>
#include <map>
#include <unordered_map>
#include <map>
#include <vector>

#include "../src/DFA.hh"
#include "../src/NFA.hh"
#include "../src/lexer_common.hh"

size_t counter = 0;

using lexer::symbol;

template<typename T1, typename T2>
bool agree(T1 &m1, T2 &m2, std::string &x, std::string error) {
  if (m1.accept(x) != m2.accept(x)) {
    std::cout << error << std::endl;
    return false;
  }
  return true;
}

void printResult(bool success) {
  std::cout << "Test case #" << counter++ << ":\t" << (success?"Pass":"Fail") << std::endl;
}

// test accept with single accept type
void testAcceptSingle() {

}

// test accept with single accept type, but many accept states
void testAcceptMultiple() {

}

// test determinization works
void testDeterminize() {

  // Language: 0*(01)*0*
  std::multimap<std::pair<lexer::state, lexer::symbol>, lexer::state> d;
  
  d.insert(std::make_pair(std::make_pair(0,symbol('0')), 0));
  d.insert(std::make_pair(std::make_pair(0,lexer::LAMBDA), 1));
  d.insert(std::make_pair(std::make_pair(1,symbol('0')), 2));
  d.insert(std::make_pair(std::make_pair(1,lexer::LAMBDA), 3));
  d.insert(std::make_pair(std::make_pair(2,symbol( '1')), 1));
  d.insert(std::make_pair(std::make_pair(3,symbol( '0')), 3));
  
  std::unordered_map<lexer::state, lexer::acceptType> acc = {{3,1}};

  lexer::NFA automaton(4, acc, 0, d);

  lexer::DFA deterministic = std::move(automaton.determinize()); 

  std::vector<std::string> testStrs = {"", "0110", "000", "001", "0010", "01010"};

  for (auto &x : testStrs) {
    if (!agree(automaton, deterministic, x, "Error in testDeterminize():")) {
      std::cout << "Disagreed on input string: " << x << std::endl;
      return;
    }
  }

  std::cout << "testDeterminize: passed" << std::endl;

}

void testDeterminize2() {

  // should accept {01}*10
  std::multimap<std::pair<lexer::state, lexer::symbol>, lexer::state> d;

  d.insert(std::make_pair(std::make_pair(0,symbol('0')), 0));
  d.insert(std::make_pair(std::make_pair(0,symbol('1')), 0));
  d.insert(std::make_pair(std::make_pair(0,symbol('1')), 1));
  d.insert(std::make_pair(std::make_pair(1,symbol('0')), 2));

  std::unordered_map<lexer::state, lexer::acceptType> acc = {{2, 1}};

  lexer::NFA m1(3, acc, 0, d);

  lexer::DFA m2 = std::move(m1.determinize());

  // test m1 and m2 accept same strings and reject same strings.

  std::vector<std::string> accStrs = {"10", "010", "110", "00110010"};
  std::vector<std::string> rejStrs = {"", "01", "11", "0010101"};

  for (auto &x : accStrs) {
    if (!agree(m1, m2, x, "Error in testDeterminize2()")) {
      std::cout << "Disagreed on input string: " << x << std::endl;
      return;
    }
  }

  for (auto &x : rejStrs) {
    if (!agree(m1, m2, x, "Error in testDeterminize2():")) {
      std::cout << "Disagreed on input string: " << x << std::endl;
      return;
    }
  }
  
  std::cout << "testDeterminize2: passed" << std::endl;
}

int main() {

  testAcceptSingle();

  testAcceptMultiple();

  testDeterminize();

  testDeterminize2();

}
