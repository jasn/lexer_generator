#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DFA.hh"
#include "lexer_common.hh"

namespace {

  using lexer::state; using lexer::symbol; using lexer::DFA;
  using lexer::acceptType;
  
  std::map<std::pair<state, symbol>, state>
  getProductDelta(const DFA &a, const DFA &b);
  

  acceptType helper(std::unordered_map<state, acceptType> &m,
		    state idx,
		    acceptType default_) {

    auto x = m.find(idx);
    
    if (x == std::end(m)) {
      return default_;
    }
    
    return x->second;

  }

  
} // end unnamed namespace

namespace lexer {

  DFA::DFA() : numberOfStates(0), A(std::unordered_map<state, acceptType>()), 
	     q0(0), delta(std::map<std::pair<state, symbol>, state>()) {  }

  DFA::DFA(size_t numberOfStates, std::unordered_map<state, acceptType> A,
	 state initialState, std::map<std::pair<state, symbol>, state> delta) :
    numberOfStates(numberOfStates), A(std::move(A)), q0(initialState), delta(std::move(delta)) {}

  acceptType DFA::accept(std::string &s) const {
    state currentState = q0;

    for (auto c : s) {
      auto nextState = delta.find(std::make_pair(currentState, c));
      if (nextState == delta.end()) {
	// no such edge from current state, i.e. crash occurs.
	return 0;
      }
      currentState = nextState->second;
    }

   
    auto ret = A.find(currentState);
    return ret == std::end(A) ? 0 : ret->second;

  }


  DFA DFA::join(const DFA &a, const DFA &b) {

    std::unordered_map<state, acceptType> newAccept;

    // need to add a crash state for both machines.
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta(::getProductDelta(a, b));
    
    for (auto x : a.A) {
      for (size_t i = 0; i < b.numberOfStates+1; ++i) {
	newAccept[x.first*(b.numberOfStates+1) + i] = x.second;
	//newAcceptStates.push_back(x.first*(b.numberOfStates+1) + i);
      }
    }

    for (auto x : b.A) {
      for (size_t i = 0; i < a.numberOfStates+1; ++i) {
	state idx = i*(b.numberOfStates+1) + x.first;
	newAccept[idx] = std::max(newAccept[idx], x.second);
      }
    }

    return DFA(numberOfStates, newAccept, 0, newDelta);

  }

  DFA DFA::intersection(const DFA &a, const DFA &b) {
    std::unordered_map<state, acceptType> newAccept;

    // need to add a crash state for both machines.
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta(::getProductDelta(a, b));
    
    for (auto x : a.A) {
      for (auto y : b.A) {
	state idx = x.first*(b.numberOfStates+1)+y.first;
	newAccept[idx] = std::max(x.second, y.second);
      }
    }

    return DFA(numberOfStates, newAccept, 0, newDelta);

  }
  

  DFA DFA::minus(const DFA &a, const DFA &b) {
    std::unordered_map<state, acceptType> newAccept;

    // need to add a crash state for both machines.
    size_t numberOfStates = (a.getNumberOfStates()+1) * (b.getNumberOfStates()+1);
    std::map<std::pair<state, symbol>, state> newDelta(::getProductDelta(a, b));
    
    for (auto x : a.A) {
      for (state bi = 0; bi < b.numberOfStates + 1; ++bi) {
	if (!b.A.at(bi)) {
	  newAccept[x.first*(b.numberOfStates+1) + bi] = x.second;
	}
      }
    }

    return DFA(numberOfStates, newAccept, 0, newDelta);
    
  }

  void DFA::minimize() {

    // eliminate unreachable states
    std::unordered_set<state> reachableStates;
    
    std::queue<state> Q;
    Q.push(q0);
    reachableStates.insert(q0);
    while (!Q.empty()) {
      state curr = Q.front(); Q.pop();
      
      // iterate through edges
      auto it = delta.lower_bound(std::make_pair(curr, std::numeric_limits<symbol>::min()));
      while (it->first.first == curr) {
	if (reachableStates.find(it->second) == std::end(reachableStates)) {
	  reachableStates.insert(it->second);
	  Q.push(it->second);
	}
	++it;
      }
    }


    // Find initial eq classes: reject and all accept types
    std::vector<std::vector<bool>> eqClasses(this->numberOfStates, std::vector<bool>(this->numberOfStates, false));

    for (state i = 0; i < eqClasses.size(); ++i) {
      if (reachableStates.find(i) == std::end(reachableStates)) continue;

      for (state j = i+1; j < eqClasses.size(); ++j) {
	if (reachableStates.find(j) == std::end(reachableStates)) continue;

	acceptType it = ::helper(A, i, lexer::REJECT);
	acceptType jt = ::helper(A, j, lexer::REJECT);

	if (it != jt) {
	  eqClasses[i][j] = true;
	}
      }
    }


    // Iteratively find equivalence classes.
    // Fixed point computation.
    bool done = false;
    std::unordered_set<symbol> alphabet = this->getAlphabet();
    while (!done) {
      done = true;
      for (state i = 0; i < eqClasses.size(); ++i) {
	if (reachableStates.find(i) == std::end(reachableStates)) continue;

	for (state j = i+1; j < eqClasses.size(); ++j) {
	  if (reachableStates.find(j) == std::end(reachableStates)) continue;

	  for (auto s : alphabet) {
	    if (delta.find(std::make_pair(i, s)) == std::end(delta) ||
		delta.find(std::make_pair(j, s)) == std::end(delta)) {
	      continue;
	    }
	    
	    state si = delta.find(std::make_pair(i, s))->second;
	    state sj = delta.find(std::make_pair(j, s))->second;

	    bool before = eqClasses[i][j];
	    eqClasses[i][j] = eqClasses[si][sj] || eqClasses[sj][si] || eqClasses[i][j];
	    
	    if (!before && eqClasses[i][j]) done = false;
	  }
	}
      }      
    }


    // Compute new states, one for each equivalence class
    // Create mappings between old and new states.
    uint32_t counter = 1;
    std::unordered_map<state, std::vector<state> > newToOlds;
    std::unordered_map<state, state> oldToNew;
    newToOlds.insert(std::make_pair(0, std::vector<state>(1,0)));
    oldToNew[q0] = 0;

    for (size_t j = 1; j < eqClasses.size(); ++j) {
      bool newState = true;
      for (state i = 0; i < j; ++i) {
	if (!eqClasses[i][j]) {
	  // j and i are in the same eq class
	  newToOlds[oldToNew[i]].push_back(j);
	  oldToNew[j] = oldToNew[i];
	  newState = false;
	}
      }
      if (newState) {
	newToOlds.insert(std::make_pair(counter, std::vector<state>(1, j)));
	oldToNew[j] = counter;
	++counter;
      }
    }

    // Compute new transition function
    std::map<std::pair<state, symbol>, state> newDelta;

    for (auto x : newToOlds) {
      state start = x.first;
      for (auto s : alphabet) {
	state oldStart = x.second[0];
	if (delta.find(std::make_pair(oldStart, s)) == std::end(delta)) continue;
	state oldEnd = delta.find(std::make_pair(oldStart, s))->second;
	state target = oldToNew[oldEnd];
	newDelta.insert(std::make_pair(std::make_pair(start, s), target));

      }
    }


    // Compute new accept states
    std::unordered_map<state, acceptType> newAccepts;

    for (auto x : A) {
      state n = oldToNew[x.first];
      newAccepts[n] = std::max(newAccepts[n], x.second);
    }
    
    this->A = std::move(newAccepts);

    this->delta = std::move(newDelta);

    this->q0 = 0;

    this->numberOfStates = counter;

  }

  std::string DFA::toDot() const {
    std::stringstream ss;
    ss << "digraph M {" << std::endl;

    for (auto x : this->A) {
      if (x.second)
	ss << "s" << x.first << "[ color=blue ];" << std::endl;
    }
    
    for (auto x : this->delta) {
      ss << "s" << x.first.first << " -> s" << x.second
	 << " [ label=\"" << x.first.second <<"\" ];" << std::endl;
    }

    ss << "}";
    return ss.str();
  }  

  const std::unordered_map<state, acceptType>& DFA::getAcceptStates() const {
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
