#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "FA.hh"


namespace lexer {

  FA::FA() : numberOfStates(0), A(std::vector<state>()), 
	     q0(0), delta(std::map<std::pair<state, symbol>, state>()) {  }

  FA::FA(size_t numberOfStates, std::vector<state> &acceptStates,
	 state initialState, std::map<std::pair<state, symbol>, state> &delta) :
    numberOfStates(numberOfStates), A(acceptStates), q0(q0), delta(delta) {}

  bool FA::accept(std::string &s) {
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
  
  std::vector<state>& FA::getAcceptStates() {
    return A;
  }

  state FA::getInitialState() {
    return q0;
  }
  // FA concat(FA &lhs, FA &rhs) {

  //   size_t lhsSize lhs.numberOfStates;
  //   size_t rhsSize rhs.numberOfStates;
  //   size_t concatSize = lhsSize + rhsSize;
    
  //   vector<state> lhsAcceptStates(lhs.A);
  //   vector<state> rhsAcceptStates(rhs.A);

  //   vector<state> concatAcceptStates;
  //   for (auto x : rhsAcceptStates) {
  //     concatAcceptStates.push_back(x + lhsSize);
  //   }

    
  //   std::map<std::pair<state, symbol>, state> concatDelta(lhs.delta);

  //   for (auto x : rhs.delta) {
  //     concatDelta.insert(make_pair(make_pair(x.first.first + lhsSize, x.first.second),
  // 				   x.second+lhsSize))
  //   }

  //   for (auto x : lhsAcceptStates) {

  //     auto it = lhs.delta.lower_bound(make_pair(x, 0));
  //     while (x == it->first.first) {
  // 	auto symb = it->first.second;
  // 	concatDelta.insert(make_pair(make_pair(x, symb),
  // 				     rhs.q0+lhsSize))
  // 	++it;
  //     }
      
  //   }

  //   res.
  // }

} // end namespace lexer


