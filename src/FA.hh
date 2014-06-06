#ifndef FA_HH_GUARD
#define FA_HH_GUARD

#include <map>
#include <stdint.h>
#include <string>
#include <vector>



namespace lexer {

  typedef uint32_t state;
  typedef uint8_t symbol;

class FA {
public:

  FA();

  FA(size_t numberOfStates, std::vector<state> &acceptStates,
     state initialState, std::map<std::pair<state, symbol>, state> &delta);

  bool accept(std::string &s);

  std::vector<state>& getAcceptStates();

  state getInitialState();

private:
  // alphabet is implicit
  size_t numberOfStates;
  std::vector<state> A; // Accept States. Invariant: A is always sorted.
  state q0; // initial state
  std::map<std::pair<state, symbol>, state> delta; // transition function


};


} // end lexer namespace

#endif
