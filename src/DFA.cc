#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include "DFA.hh"

#include <iostream>

namespace {

  using lexer::state; using lexer::symbol; using lexer::DFA;
  
  std::map<std::pair<state, symbol>, state>
  getProductDelta(const DFA &a, const DFA &b);

}

namespace lexer {

  DFA::DFA() : numberOfStates(0), A(std::vector<state>()), 
	     q0(0), delta(std::map<std::pair<state, symbol>, state>()) {  }

  DFA::DFA(size_t numberOfStates, std::vector<state> &acceptStates,
	 state initialState, std::map<std::pair<state, symbol>, state> &delta) :
    numberOfStates(numberOfStates), A(acceptStates), q0(initialState), delta(delta) {}

  bool DFA::accept(std::string &s) const {
    state currentState = q0;

    for (auto c : s) {
      auto nextState = delta.find(std::make_pair(currentState, c));
      if (nextState == delta.end()) {
	// no such edge from current state, i.e. crash occurs.
	return false;
      }
      currentState = nextState->second;
    }

    return std::binary_search(std::begin(A), std::end(A), currentState);
  }


  std::string DFA::toDot() const {
    std::stringstream ss;
    ss << "digraph M {" << std::endl;

    for (auto x : this->A) {
      ss << "s" << x << "[ color=blue ];" << std::endl;
    }
    
    for (auto x : this->delta) {
      ss << "s" << x.first.first << " -> s" << x.second
	 << " [ label=\"" << x.first.second <<"\" ];" << std::endl;
    }

    ss << "}";
    return ss.str();
  }
  
  DFA DFA::join(const DFA &a, const DFA &b) {
    std::vector<state> newAcceptStates;
    // need to add a crash state for both machines.
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta(::getProductDelta(a, b));
    
    for (auto x : a.A) {
      for (size_t i = 0; i < b.numberOfStates+1; ++i) {
	newAcceptStates.push_back(x*(b.numberOfStates+1) + i);
      }
    }

    for (auto x : b.A) {
      for (size_t i = 0; i < a.numberOfStates+1; ++i) {
	newAcceptStates.push_back(i*(b.numberOfStates+1) + x);
      }
    }

    std::sort(std::begin(newAcceptStates), std::end(newAcceptStates));
    
    return DFA(numberOfStates, newAcceptStates, 0, newDelta);

  }

  DFA DFA::intersection(const DFA &a, const DFA &b) {
    std::vector<state> newAcceptStates;
    // need to add a crash state for both machines.
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta(::getProductDelta(a, b));
    
    for (auto x : a.A) {
      for (auto y : b.A) {
	newAcceptStates.push_back(x*(b.numberOfStates+1) + y);	
      }
    }

    std::sort(std::begin(newAcceptStates), std::end(newAcceptStates));
    
    return DFA(numberOfStates, newAcceptStates, 0, newDelta);

  }
  

  DFA DFA::minus(const DFA &a, const DFA &b) {
    std::vector<state> newAcceptStates;
    // need to add a crash state for both machines.
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta(::getProductDelta(a, b));
    
    for (auto x : a.A) {
      for (size_t i = 0; i < b.A.size(); ++i) {
	if (!std::binary_search(std::begin(b.A), std::end(b.A), i)) {
	  newAcceptStates.push_back(x*(b.numberOfStates+1) + i);		  
	}
      }
    }

    std::sort(std::begin(newAcceptStates), std::end(newAcceptStates));
    
    return DFA(numberOfStates, newAcceptStates, 0, newDelta);
    
  }

  const std::vector<state>& DFA::getAcceptStates() const {
    return A;
  }

  const std::map<std::pair<state, symbol>, state>&
  DFA::getDelta() const {
    return delta;
  }


  state DFA::getInitialState() {
    return q0;
  }


  std::unordered_set<symbol> DFA::getAlphabet() const {
    std::unordered_set<symbol> res;
    for (auto x : this->delta) {
      res.insert(x.first.second);
    }
    return res;
  }

  size_t DFA::getNumberOfStates() const {
    return this->numberOfStates;
  }
  

} // end namespace lexer


namespace {

  using lexer::state; using lexer::symbol; using lexer::DFA;
  
  std::map<std::pair<state, symbol>, state>
  getProductDelta(const DFA &a, const DFA &b) {
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta;
    

    std::unordered_set<symbol> aAlpha = a.getAlphabet();
    std::unordered_set<symbol> bAlpha = b.getAlphabet();

    std::unordered_set<symbol> newAlphabet(std::begin(aAlpha), std::end(aAlpha));
    newAlphabet.insert(std::begin(bAlpha), std::end(bAlpha));

    for (state ai = 0; ai < a.getNumberOfStates(); ++ai) {
      for (state bi = 0; bi < b.getNumberOfStates(); ++bi) {
	state currentState = ai*(b.getNumberOfStates()+1) + bi;
	
	for (auto s : newAlphabet) {
	  state as, bs;

	  auto aTransition = a.getDelta().find(std::make_pair(ai, s));
	  auto bTransition = b.getDelta().find(std::make_pair(bi, s));

	  if (aTransition == std::end(a.getDelta())) {
	    as = a.getNumberOfStates()+1;
	  } else {
	    as = aTransition->second;
	  }
	  if (bTransition == std::end(b.getDelta())) {
	    bs = b.getNumberOfStates()+1;
	  } else {
	    bs = bTransition->second;
	  }

	  state resultState = as * (b.getNumberOfStates()+1) + bs;

	  newDelta[std::make_pair(currentState, s)] = resultState;

	}
      }
    }

    return newDelta;
    
  }
} // end noname namespace
