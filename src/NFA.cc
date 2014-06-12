#include "NFA.hh"
#include "lexer_common.hh"

#include <map>
#include <unordered_map>
#include <unordered_set>

namespace lexer {

  NFA::NFA(std::multimap<std::pair<state, symbol>, state> delta,
           std::unordered_map<state, acceptType> A,
           size_t numberOfStates, state q0) : delta(std::move(delta)), A(std::move(A)), 
                                              numberOfStates(numberOfStates), q0(q0) { }

  acceptType NFA::accept(std::string &x) const {

    std::unordered_set<state> currStateSpace = {q0};
    
    for (auto c : x) {
      std::unordered_set<state> nextStateSpace;
      for (auto y : currStateSpace) {
        auto its = delta.equal_range(std::make_pair(y, c));
        while (its.first != its.second) {
          nextStateSpace.insert((its.first)->second);
          ++(its.first);
        }
        
        its = delta.equal_range(std::make_pair(y, LAMBDA));
        while (its.first != its.second) {
          nextStateSpace.insert((its.first)->second);
          ++(its.first);
        }
      }

      std::swap(nextStateSpace, currStateSpace);
    }
    

    acceptType res = lexer::REJECT;
    for (auto x : currStateSpace) {
      auto y = A.find(x);
      if (y != std::end(A) && y->second > res) {
        res = y->second;
      }
    }

    return res;
  }

  void NFA::lambdaElimination() {
    return;
  }
    
  DFA NFA::determinize() {
    return DFA();
  }


}
