#ifndef NFA_HH_GUARD
#define NFA_HH_GUARD

#include "lexer_common.hh"
#include "DFA.hh"

#include "stdint.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace lexer {

  class NFA {
  private:
    std::multimap<std::pair<state, symbol>, state> delta;
    std::unordered_map<state, acceptType> A;
    size_t numberOfStates;
    state q0;
    void lambdaElimination();

  public:
    
    NFA(size_t numberOfStates, std::unordered_map<state, acceptType> A,
	state q0, std::multimap<std::pair<state, symbol>, state> delta);
	
  
    const std::multimap<std::pair<state, symbol>, state>&
    getDelta() const;

    acceptType accept(std::string &s) const;
    
    DFA determinize();
    
    std::string toDot() const;
  };
  
} // end lexer namespace


#endif // NFA_HH_GUARD
