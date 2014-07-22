#ifndef DFA_HH_GUARD
#define DFA_HH_GUARD

#include <map>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "lexer_common.hh"

namespace lexer {

class DFA {

public:

  typedef std::map<std::pair<state, symbol>, state> delta_type;

  DFA();

  DFA(size_t numberOfStates, std::unordered_map<state, acceptType> acceptStates,
     state initialState, delta_type delta);

  acceptType getAcceptTypeForState(const state idx, const acceptType default_) const;

  acceptType accept(const std::string &s) const;

  static DFA join(DFA const &a, DFA const &b);

  static DFA intersection(const DFA &a, const DFA &b);

  static DFA minus(const DFA &a, const DFA &b);

  void minimize();

  const std::unordered_map<state, acceptType>& getAcceptStates() const;

  const delta_type&
  getDelta() const;

  state getInitialState() const;

  size_t getNumberOfStates() const;

  std::unordered_set<symbol> getAlphabet() const;

  std::string toDot() const;

private:

  size_t numberOfStates;

  std::unordered_map<state, acceptType> A;

  state q0; // initial state
  delta_type delta; // transition function

  void addCrashState();


};


} // end lexer namespace

#endif
