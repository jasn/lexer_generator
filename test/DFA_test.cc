#include <iostream>
#include <map>
#include <vector>

#include "../src/DFA.hh"

size_t counter = 1;

void printResult(bool success) {
  std::cout << "Test case #" << counter++ << ":\t" << (success?"Pass":"Fail") << std::endl;
}

void testMinimize2() {
    std::map<std::pair<lexer::state, lexer::symbol>, lexer::state> d;
    d.insert(std::make_pair(std::make_pair(0, '0'), 0));
    d.insert(std::make_pair(std::make_pair(0, '1'), 0));
    
    std::vector<lexer::state> acc = {0};

    lexer::DFA m(1, acc, 0, d);

    std::vector<std::string> strs = {"", "0", "1", "00", "01", "10", "11" };

    for (auto x : strs) {
      if (!m.accept(x)) {
	std::cout << "Failed test 'Minimize 2': Before minimize m Did not accept string:" << x << std::endl;
	return;
      }
    }

    m.minimize();

    for (auto x : strs) {
      if (!m.accept(x)) {
	std::cout << "Failed test 'Minimize 2': After minimize m Did not accept string:" << x << std::endl;
	return;
      }
    }

    std::cout << "'Minimize 2' passed" << std::endl;

}

void testMinimize1() {
  std::cout << "###########################################" << std::endl;
  std::cout << "######## Starting Test Minimize ###########" << std::endl;
  std::cout << "###########################################" << std::endl;

  std::map<std::pair<lexer::state, lexer::symbol>, lexer::state> d;
  // recognizes {0,1}*10
  d.insert(std::make_pair(std::make_pair(0, '0'), 1));
  d.insert(std::make_pair(std::make_pair(0, '1'), 2));
  d.insert(std::make_pair(std::make_pair(1, '0'), 3));
  d.insert(std::make_pair(std::make_pair(1, '1'), 4));
  d.insert(std::make_pair(std::make_pair(2, '0'), 5));
  d.insert(std::make_pair(std::make_pair(2, '1'), 6));
  d.insert(std::make_pair(std::make_pair(3, '0'), 3));
  d.insert(std::make_pair(std::make_pair(3, '1'), 4));
  d.insert(std::make_pair(std::make_pair(4, '0'), 5));
  d.insert(std::make_pair(std::make_pair(4, '1'), 6));
  d.insert(std::make_pair(std::make_pair(5, '0'), 3));
  d.insert(std::make_pair(std::make_pair(5, '1'), 4));
  d.insert(std::make_pair(std::make_pair(6, '0'), 5));
  d.insert(std::make_pair(std::make_pair(6, '1'), 6));

  std::vector<lexer::state> acc = {5};
  
  lexer::DFA m(7, acc, 0, d);

  // verify it accepts {0,1}*10
  std::vector<std::string> strs = {"00010", "10", "1010", "010101010"};

  for (auto x : strs) {
    if (!m.accept(x)) {
      std::cout << "Failed test 'Minimize 1': Before minimize m Did not accept string:" << x << std::endl;
      return;
    }
  }

  m.minimize();

  for (auto x : strs) {
    if (!m.accept(x)) {
      std::cout << "Failed test 'Minimize 1': After minimize m Did not accept string: " << x << std::endl;
      std::cout << m.toDot() << std::endl;
      return;
    }
  }
  
  if (m.getNumberOfStates() != 3) {
    std::cout << "Failed test 'Minimize 1': Incorrect number of states in minimized automaton" << std::endl;
    return;
  }

  std::cout << "'Minimize 1' passed" << std::endl;

}

void testMinus() {
  std::cout << "No tests for minus yet." << std::endl;
}

void testIntersection() {
  std::cout << "No tests for intersection yet." << std::endl;
}

void testJoin() {
  std::cout << "###########################################" << std::endl;
  std::cout << "########## Starting Test Join #############" << std::endl;
  std::cout << "###########################################" << std::endl;

  std::map<std::pair<lexer::state, lexer::symbol>, lexer::state> d1;
  d1.insert(std::make_pair(std::make_pair(0, '0'), 1));
  d1.insert(std::make_pair(std::make_pair(0, '1'), 0));
  d1.insert(std::make_pair(std::make_pair(1, '0'), 2));
  d1.insert(std::make_pair(std::make_pair(1, '1'), 0));
  d1.insert(std::make_pair(std::make_pair(2, '0'), 2));
  d1.insert(std::make_pair(std::make_pair(2, '1'), 0));
  std::vector<lexer::state> acc = {2};

  lexer::DFA endsWithTwoZeros(3, acc, 0, d1);


  std::map<std::pair<lexer::state, lexer::symbol>, lexer::state> d2;
  d2.insert(std::make_pair(std::make_pair(0, '0'), 0));
  d2.insert(std::make_pair(std::make_pair(0, '1'), 1));
  d2.insert(std::make_pair(std::make_pair(1, '0'), 0));
  d2.insert(std::make_pair(std::make_pair(1, '1'), 2));
  d2.insert(std::make_pair(std::make_pair(2, '0'), 0));
  d2.insert(std::make_pair(std::make_pair(2, '1'), 2));


  lexer::DFA endsWithTwoOnes(3, acc, 0, d2);

  lexer::DFA joint = lexer::DFA::join(endsWithTwoOnes, endsWithTwoZeros);

  std::vector<std::string> positiveTestCases = { "01100","00","100",
						 "100100", "11", "011", "010111" };
  std::vector<std::string> negativeTestCases = { "0110","010","110","1001001" };


  for (size_t i = 0; i < positiveTestCases.size(); ++i) {
    printResult(joint.accept(positiveTestCases[i]));
  }

  for (size_t i = 0; i < negativeTestCases.size(); ++i) {
    printResult(!joint.accept(negativeTestCases[i]));
  }

}

void testAccept() {

  // Creates an automaton that accepts all binary strings that end with at least
  // two '0' characters.
  
  std::cout << "###########################################" << std::endl;
  std::cout << "############# Starting Test 1 #############" << std::endl;
  std::cout << "###########################################" << std::endl;

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

  testAccept();

  std::cout << " #### " << std::endl;

  testJoin();

  testIntersection();

  testMinus();

  testMinimize1();

  testMinimize2();

}
