 #ifndef REGULAR_EXPRESSION_HH_GUARD
#define REGULAR_EXPRESSION_HH_GUARD

#include <memory>
#include <iostream>
#include <limits>
#include <unordered_set>

#include "lexer_common.hh"
#include "NFA.hh"

namespace lexer {

class RegularExpression {

public:

  virtual void printType(std::ostream &os) {
    os << "error, visiting 'RegularExpression' type" << std::endl;
  }

  virtual lexer::NFA getNFA() const {
    return lexer::NFA(1, {{0,1}}, 0, {});    
  }
  
};

struct RegExpConcat : public RegularExpression {
  std::shared_ptr<RegularExpression> left, right;
  RegExpConcat(std::shared_ptr<RegularExpression> left, 
	       std::shared_ptr<RegularExpression> right) : left(left), right(right) {};  

  // add lambda transition for each accept in left to initial in right

  virtual void printType(std::ostream &os) {
    os << "{\"type\":\"concat\"," << std::endl;
    os << "\"left\":" << std::endl;
       
    left->printType(os);
    
    os << ",\"right\":" << std::endl;
    right->printType(os);
    os << "}" << std::endl;
    
  }

  virtual lexer::NFA getNFA() {
    return lexer::NFA(1, {{0,1}}, 0, {});
  }

};

struct RegExpPlus : public RegularExpression {
  std::shared_ptr<RegularExpression> inner;

  RegExpPlus(std::shared_ptr<RegularExpression> inner_) : inner(inner_) {};

  // add lambda transition for each accept to initial

  virtual void printType(std::ostream &os) {
    os << "{\"type\":\"plus\"," << std::endl << "\"inner\":";
    inner->printType(os);
    os << "}" << std::endl;
  }

  virtual lexer::NFA getNFA() {
    return lexer::NFA(1, {{0,1}}, 0, {});
  }

};

struct RegExpStar : public RegularExpression {
  std::shared_ptr<RegularExpression> inner;
  RegExpStar(std::shared_ptr<RegularExpression> inner_) : inner(inner_) {};

  // add lambda transition for each accept to initial and make initial accept
  virtual void printType(std::ostream &os) {
    os << "{\"type\":\"star\"," << std::endl << "\"inner\":";
    inner->printType(os);
    os << "}" << std::endl;
  }

  virtual lexer::NFA getNFA() {
    return lexer::NFA(1, {{0,1}}, 0, {});
  }

};

struct RegExpOr : public RegularExpression {
  std::shared_ptr<RegularExpression> left, right;
  RegExpOr(std::shared_ptr<RegularExpression> left, 
	       std::shared_ptr<RegularExpression> right) : left(left), right(right) {};

  // join the two automatons.
  virtual void printType(std::ostream &os) {
    os << "{\"type\":\"or\"," << std::endl;
    os << "\"left\":" << std::endl;
       
    left->printType(os);
    
    os << ",\"right\":" << std::endl;
    right->printType(os);
    os << "}" << std::endl;
  }

  virtual lexer::NFA getNFA() {
    return lexer::NFA(1, {{0,1}}, 0, {});
  }

};

struct RegExpChars : public RegularExpression {
  std::unordered_set<char> chars;

  RegExpChars(std::unordered_set<char> chars, bool invert) {
    if (invert) {
      for (char x = std::numeric_limits<char>::min();
	   x != std::numeric_limits<char>::max(); ++x) {
	if (!chars.count(x)) this->chars.insert(x);
      }
      if (!chars.count(std::numeric_limits<char>::max())) {
	this->chars.insert(std::numeric_limits<char>::max());
      }
    } else {
      this->chars = std::move(chars);
    }
  }

  // Make a two state machine that accepts exactly one occurrence of a character in chars.

  virtual void printType(std::ostream &os) {
    os << "{\"type\":\"chars\"," << std::endl;
    os << "\"value\":\"";
    for (auto x : chars) {
      if (x < 32) continue;
      if (x == '\\' || x == '"')
	os << "\\";
      os << x;
    }
    os << "\"}" << std::endl;
  }

  virtual lexer::NFA getNFA() {
    return lexer::NFA(1, {{0,1}}, 0, {});
  }

};

} // end namespace lexer

#endif // end REGULAR_EXPRESSION_HH_GUARD
