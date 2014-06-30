#include <iostream>
#include <map>
#include <unordered_map>
#include <map>
#include <vector>

#include "../src/DFA.hh"
#include "../src/NFA.hh"
#include "../src/lexer_common.hh"

size_t counter = 0;

void printResult(bool success) {
  std::cout << "Test case #" << counter++ << ":\t" << (success?"Pass":"Fail") << std::endl;
}

// test accept with single accept type
void testAcceptSingle() {

}

// test accept with single accept type
void testAcceptMultiple() {

}

// test determinization works
void testDeterminize() {

  std::multimap<std::pair<lexer::state, lexer::symbol>, lexer::state> d;

  
  d.insert(std::make_pair(std::make_pair(0,'0'), 0));
  d.insert(std::make_pair(std::make_pair(0,lexer::LAMBDA), 1));
  d.insert(std::make_pair(std::make_pair(1,'0'), 2));
  d.insert(std::make_pair(std::make_pair(1,lexer::LAMBDA), 3));
  d.insert(std::make_pair(std::make_pair(2, '1'), 1));
  d.insert(std::make_pair(std::make_pair(3, '0'), 3));
  
  std::unordered_map<lexer::state, lexer::acceptType> acc = {{3,1}};

  lexer::NFA automaton(4, acc, 0, d);

  lexer::DFA deterministic = std::move(automaton.determinize()); 

  deterministic.minimize();

  std::cout << deterministic.toDot() << std::endl;

}

int main() {

  testAcceptSingle();

  testAcceptMultiple();

  testDeterminize();

}
