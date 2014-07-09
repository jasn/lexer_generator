#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <stdint.h>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "lexer_common.hh"
#include "NFA.hh"

namespace {

  using lexer::symbol; using lexer::state; using lexer::LAMBDA;

  using lexer::acceptType;

  void addLambdaFromAcceptToInitial(std::multimap<std::pair<state, symbol>, state> &newDelta,
				    std::unordered_map<state, acceptType> &newAccepts,
				    state newInitial) {    
    for (auto x : newAccepts) {
      newDelta.insert(std::make_pair(std::make_pair(x.first, lexer::LAMBDA), newInitial));
    }

  }

  acceptType getAcceptType(std::set<state> &s, std::unordered_map<state, acceptType> A) {
    acceptType a = 0;
    for (auto x : s) {
      auto val = A.find(x);
      if (val != std::end(A)) {
	a = std::max(a, val->second);
      }
    }
    return a;
  }


  std::unordered_set<state> getLambdaClosureForState(state s, const std::multimap<std::pair<state, symbol>, state> &delta, size_t numberOfStates) {
    // fixed point computation
    std::vector<int8_t> visited(numberOfStates, 0);
    std::queue<state> Q;
    Q.push(s);
    visited[s] = true;
    
    while (!Q.empty()) {
      state next = Q.front();
      Q.pop();

      auto iters = delta.equal_range(std::make_pair(next, lexer::LAMBDA));

      for (auto it = iters.first; it != iters.second; ++it) {
	state target = it->second;
	if (!visited[target]) {
	  Q.push(target);
	  visited[target] = true;
	}
      }
    }

    std::unordered_set<state> ret;
    for (state i = 0; i < numberOfStates; ++i) {
      if (visited[i]) ret.insert(i);
    }
    return ret;
  }

} // end noname namespace

namespace lexer {

  NFA::NFA(size_t numberOfStates, std::unordered_map<state, acceptType> A,
           state q0, std::multimap<std::pair<state, symbol>, state> delta) : 
    numberOfStates(numberOfStates), A(std::move(A)), q0(q0), delta(std::move(delta)) { }

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

	// only takes 1 lambda step, there could be multiple...
	// essentially need to do bfs through lambdas.
	// probably should do that at some point..
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

  NFA NFA::concat(const NFA &a, const NFA &b) {

    size_t aSize = a.getNumberOfStates();
    size_t bSize = b.getNumberOfStates();

    auto aDelta = a.getDelta();
    auto bDelta = b.getDelta();

    std::multimap<std::pair<state, symbol>, state> newDelta;
    std::unordered_map<state, acceptType> newAccepts;

    size_t newSize = aSize + bSize;

    newDelta.insert(std::begin(aDelta), std::end(aDelta));
    
    for (auto x : bDelta) {
      newDelta.insert(std::make_pair(std::make_pair(x.first.first+aSize, x.first.second),
				     x.second + aSize));
    }

    state bNewInitial = b.getInitialState() + aSize;
    
    for (auto x : a.getAcceptStates()) {
      newDelta.insert(std::make_pair(std::make_pair(x.first, lexer::LAMBDA), 
				     bNewInitial));
    }

    for (auto x : b.getAcceptStates()) {
      newAccepts.insert({x.first+aSize, x.second});
    }

    return NFA(newSize, newAccepts, a.getInitialState(), newDelta);

  }

  NFA NFA::addStar(const NFA &a, acceptType at) {
    std::multimap<std::pair<state, symbol>, state> newDelta(a.getDelta());
    std::unordered_map<state, acceptType> newAccepts(a.getAcceptStates());
    size_t newSize = a.getNumberOfStates();
    state newInitial = a.getInitialState();
    
    addLambdaFromAcceptToInitial(newDelta, newAccepts, newInitial);
    
    if (newAccepts.count(newInitial) == 0) {
      newAccepts[newInitial] = at;
    }

    return NFA(newSize, newAccepts, newInitial, newDelta);
  }

  NFA NFA::addPlus(const NFA &a) {
    std::multimap<std::pair<state, symbol>, state> newDelta(a.getDelta());
    std::unordered_map<state, acceptType> newAccepts(a.getAcceptStates());
    size_t newSize = a.getNumberOfStates();
    state newInitial = a.getInitialState();

    addLambdaFromAcceptToInitial(newDelta, newAccepts, newInitial);
    return NFA(newSize, newAccepts, newInitial, newDelta);
  }

  NFA NFA::join(const NFA &a, const NFA &b) {

    
    std::multimap<std::pair<state, symbol>, state> newDelta;
    std::unordered_map<state, acceptType> newAccepts;

    size_t newSize = a.getNumberOfStates() + b.getNumberOfStates() + 1;
    size_t newInitial = 0;
    size_t aSize = a.getNumberOfStates();

    state aInitial = a.getInitialState();
    state bInitial = b.getInitialState();

    for (auto x : a.getDelta()) {
      newDelta.insert(std::make_pair(std::make_pair(x.first.first+1, x.first.second), 
				     x.second+1));
    }

    for (auto x : b.getDelta()) {
      newDelta.insert(std::make_pair(std::make_pair(x.first.first+1+aSize, x.first.second), 
				     x.second+1+aSize));
    }

    newDelta.insert(std::make_pair(std::make_pair(newInitial, lexer::LAMBDA), 
				   aInitial+1));
    newDelta.insert(std::make_pair(std::make_pair(newInitial, lexer::LAMBDA), 
				   bInitial+1+aSize));

    for (auto x : a.getAcceptStates()) {
      newAccepts.insert({x.first+1, x.second});
    }

    for (auto x : b.getAcceptStates()) {
      newAccepts.insert({x.first+1+aSize, x.second});
    }

    return NFA(newSize, newAccepts, newInitial, newDelta);
  }

  NFA NFA::simpleAccept(std::unordered_set<symbol> accSymbols, acceptType at) {
    std::multimap<std::pair<state, symbol>, state> newDelta;
    std::unordered_map<state, acceptType> newAccepts;

    size_t newSize = 2;
    state newInitial = 0;

    for (auto x : accSymbols) {
      newDelta.insert({{0, x}, 1});
    }

    newAccepts[1] = at;

    return NFA(newSize, newAccepts, newInitial, newDelta);
  }

  const size_t NFA::getNumberOfStates() const {
    return this->numberOfStates;
  }

  const state NFA::getInitialState() const {
    return this->q0;
  }

  const std::unordered_map<state, acceptType> &
  NFA::getAcceptStates() const {
    return A;
  }

  void NFA::lambdaElimination() {
    // for each state find out where we can go using only lambdas
    std::vector<std::unordered_set<state> > lambdaClosures(numberOfStates);

    for (state s = 0; s < numberOfStates; ++s) {
    
      lambdaClosures[s] = ::getLambdaClosureForState(s, delta, numberOfStates);
      
    }

    std::multimap<std::pair<state, symbol>, state> deltaNew;
    
    for (state s = 0; s < numberOfStates; ++s) {
      for (auto x : lambdaClosures[s]) {
	// get all edges out of x -- note s is included in lambdaClosures[s]
	auto it = delta.lower_bound(std::make_pair(x, std::numeric_limits<symbol>::min()));
	auto itEnd = delta.upper_bound(std::make_pair(x, std::numeric_limits<symbol>::max()));
	
	while (it != itEnd) {
	  
	  if (it->first.second != lexer::LAMBDA) {
	    state start = s;
	    state end = it->second;
	    symbol symb = it->first.second;
	    // take one step that is *not* lambda, and add edge to that state
	    deltaNew.insert(std::make_pair(std::make_pair(start, symb), end));
	    for (auto y : lambdaClosures[end]) { // followed by arbitrary lambda steps too
	      deltaNew.insert(std::make_pair(std::make_pair(start, symb), y));
	    }
	  }
	  ++it;
	}
      }
    }

    // delete duplicates in deltaNew

    std::vector<std::pair<std::pair<state, symbol>, state> > 
      tmp(std::begin(deltaNew), std::end(deltaNew));
    
    std::sort(std::begin(tmp), std::end(tmp));

    deltaNew.clear();

    auto prev = std::begin(tmp);
    auto curr = std::begin(tmp);
    if (tmp.size() > 0) {
      std::advance(curr, 1);
    }
    
    while (curr != std::end(tmp)) {
      if (prev->first.first == curr->first.first &&
    	  prev->first.second == curr->first.second &&
    	  prev->second == curr->second) {
	++curr;
      } else {
	deltaNew.insert(*prev);
	prev = curr;
	++curr;
      }
    }
    if (tmp.size() > 0) {
      deltaNew.insert(*prev);
    }

    delta = std::move(deltaNew);

    // compute new accept states. I.e. everywhere a state can reach an accept state
    // using only lambda transitions.

    acceptType a = 0;
    for (auto x : lambdaClosures[q0]) {
      auto y = A.find(x);
      if (y != std::end(A)) {
	a = std::max(a, y->second);      
      }
    }
    if (a != 0) {
      A[q0] = a;
    }

  }
    
  DFA NFA::determinize() {

    // First find all reachable accepting states from starting state
    // by lambda transitions.

    std::unordered_set<state> q0LambdaClosure = ::getLambdaClosureForState(q0, this->delta, numberOfStates);
    acceptType a = 0;
    for (auto x : q0LambdaClosure) {
      auto y = A.find(x);
      if (y != std::end(A)) {
	a = std::max(a, y->second);
      }
    }

    this->lambdaElimination();

    // fixed point computation for new transition function: (we do it by bfs)
    //   add q0 as a state, and for each symbol add the set it can reach using that symbol
    //   as a state.
    //   Do passes through transition function adding new sets until the function
    //   is a fixed point.

    std::unordered_set<symbol> alphabet = lexer::getAlphabet(*this);

    std::map<std::pair<std::set<state>, symbol>, std::set<state> > tempNewDelta;

    {
      std::queue<std::set<state> > Q;
      Q.push(std::set<state>());
      Q.front().insert(q0);
    
      while (!Q.empty()) {
	std::set<state> e = Q.front();
	for (auto c : alphabet) {
	  std::set<state> targetSet;
	  for (auto x : e) {
	    auto iterPair = delta.equal_range(std::make_pair(x, c));
	    for (; iterPair.first != iterPair.second; ++iterPair.first) {
	      targetSet.insert(iterPair.first->second);
	    }
	  }
	  if (targetSet.size()) {
	    auto val = tempNewDelta.insert(std::make_pair(std::make_pair(e, c),
							  targetSet));
	    if (val.second) {
	      Q.push(targetSet);
	    }
	  }
	}
	Q.pop();
      }
    }

    // Rename all 'new' states to 0,1,...size-1 and build proper delta.
    std::map<std::pair<state, symbol>, state> properDelta;
    std::unordered_map<state, acceptType> properAcceptTypes;

    size_t bfsTime = 0;
    std::map<std::set<state>, size_t > visitedTime;
    visitedTime.insert(std::make_pair(std::set<state>({q0}), 0));
    std::queue<std::pair<std::set<state>, size_t> > Q;
    Q.push(std::make_pair(std::set<state>({q0}), bfsTime));

    while (!Q.empty()) {
      std::pair<std::set<state>, size_t> &e = Q.front();
      acceptType a = ::getAcceptType(e.first, A);
      if (a != 0) {
	properAcceptTypes[visitedTime[e.first]] = a;
      }
      size_t prevTime = e.second;
      for (auto c : alphabet) {
	auto x = tempNewDelta.find(std::make_pair(e.first, c));
	
	if (x != std::end(tempNewDelta)) {
	  if (visitedTime.find(x->second) == std::end(visitedTime)) {
	    visitedTime[x->second] = ++bfsTime;
	    Q.push(std::make_pair(x->second, bfsTime));
	  }
	  properDelta[std::make_pair(visitedTime[e.first], c)] = visitedTime[x->second];
	}
      }
      Q.pop();
    }

    return DFA(visitedTime.size(), properAcceptTypes, 0, properDelta);
  }

  const std::multimap<std::pair<state, symbol>, state>&
  NFA::getDelta() const {
    return delta;
  }

  std::string NFA::toDot() const {
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


} // end namespace lexer
