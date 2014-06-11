#ifndef DFA_HH_GUARD
#define DFA_HH_GUARD

#include <map>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>



namespace lexer {

  typedef uint32_t state;
  typedef uint8_t symbol;
  typedef uint32_t acceptType;

class DFA {
public:

  static const acceptType REJECT = 0;

  DFA();

  DFA(size_t numberOfStates, std::unordered_map<state, acceptType> acceptStates,
     state initialState, std::map<std::pair<state, symbol>, state> delta);

  acceptType accept(std::string &s) const;

  static DFA join(DFA const &a, DFA const &b);

  static DFA intersection(const DFA &a, const DFA &b);

  static DFA minus(const DFA &a, const DFA &b);

  void minimize();

  const std::unordered_map<state, acceptType>& getAcceptStates() const;

  const std::map<std::pair<state, symbol>, state>&
  getDelta() const;

  state getInitialState();

  size_t getNumberOfStates() const;

  std::unordered_set<symbol> getAlphabet() const;

  std::string toDot() const;

private:

  size_t numberOfStates;


  std::unordered_map<state, acceptType> A;

  state q0; // initial state
  std::map<std::pair<state, symbol>, state> delta; // transition function
  


};


} // end lexer namespace

#endif
