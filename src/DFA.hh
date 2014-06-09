#ifndef DFA_HH_GUARD
#define DFA_HH_GUARD

#include <map>
#include <stdint.h>
#include <string>
#include <unordered_set>
#include <vector>



namespace lexer {

  typedef uint32_t state;
  typedef uint8_t symbol;

class DFA {
public:

  DFA();

  DFA(size_t numberOfStates, std::vector<state> &acceptStates,
     state initialState, std::map<std::pair<state, symbol>, state> &delta);

  bool accept(std::string &s) const;

  static DFA join(DFA const &a, DFA const &b);

  static DFA intersection(const DFA &a, const DFA &b);

  static DFA minus(const DFA &a, const DFA &b);

  void minimize();

  const std::vector<state>& getAcceptStates() const;

  const std::map<std::pair<state, symbol>, state>&
  getDelta() const;

  state getInitialState();

  size_t getNumberOfStates() const;

  std::unordered_set<symbol> getAlphabet() const;

  std::string toDot() const;

private:

  size_t numberOfStates;

  std::vector<state> A; // Accept States. Invariant: A is always sorted.
  state q0; // initial state
  std::map<std::pair<state, symbol>, state> delta; // transition function
  


};


} // end lexer namespace

#endif
