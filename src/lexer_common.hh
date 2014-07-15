#ifndef LEXER_COMMON_HH_GUARD
#define LEXER_COMMON_HH_GUARD

#include <algorithm>
#include <cctype>
#include <functional>
#include <limits>
#include <set>
#include <stdint.h>
#include <unordered_set>
#include <vector>
#include <ostream>

namespace lexer {
  struct symbol {
    typedef uint8_t value_type;
    value_type val;
    bool lambda;

    friend std::ostream& operator<<(std::ostream &os, const symbol &s) {
      if (s.lambda) {
	os << "\u03BB"; // lambda
      } else {
	if (isprint(s.val)) {
	  if (s.val == '"') os << '\\';
	  os << s.val;
	} else {
	  //os << "\u2603"; // snowman
	}
      }
      return os;
    }
    
    explicit symbol(value_type s) : val(s), lambda(false) {};

    explicit symbol(value_type s, bool lambda) : val(s), lambda(lambda) {};

    static symbol min() {
      return symbol(0, true);
    }

    static symbol max() {
      return symbol(std::numeric_limits<value_type>::max(), false);
    }

    template<typename OP>
    bool compare(OP o, const symbol &rhs) const {
      if (this->lambda != rhs.lambda) {
	return o(rhs.lambda, this->lambda);
      }
      return o(this->val, rhs.val);
    }

    bool operator<(const symbol &other) const {
      return compare(std::less<void>(), other);
    }

    bool operator>(const symbol &other) const {
      return compare(std::greater<void>(), other);
    }

    bool operator==(const symbol &other) const {
      return compare(std::equal_to<void>(), other);
    }

    bool operator!=(const symbol &other) const {
      return compare(std::not_equal_to<void>(), other);
    }

    bool operator<=(const symbol &other) const {
      return compare(std::less_equal<void>(), other);
    }

    bool operator>=(const symbol &other) const {
      return compare(std::greater_equal<void>(), other);
    }

    symbol& operator++() {
      if (this->lambda) {
	this->lambda = false;
	this->val = 0;
	return *this;
      }

      if (this->val == std::numeric_limits<value_type>::max()) {
	this->lambda = true;
	this->val = 0;
	return *this;
      }
      
      ++(this->val);
      return *this;

    }

  };
} //namespace lexer

namespace std {
template<>
struct hash<lexer::symbol>: public std::unary_function<lexer::symbol, size_t> {
  hash<uint8_t> hash_fn;
  size_t operator()(const lexer::symbol &s) const noexcept {
    return hash_fn(s.val) + s.lambda*13;
  }
};
} //namespace std

namespace lexer {
  typedef uint32_t state;
  typedef uint32_t acceptType;
  
  const acceptType REJECT = 0;

  const symbol LAMBDA{0, true};

  template <typename T>
  const std::unordered_set<symbol> getAlphabet(const T &FA) {
    
    std::unordered_set<symbol> res;
    for (auto x : FA.getDelta()) {
      res.insert(x.first.second);
    }
    return res;
  }

template<typename iter_type>
struct __symbol_writer {

  iter_type begin;
  iter_type end;

  friend std::ostream& operator<<(std::ostream &os, const __symbol_writer &s) {
    std::vector<symbol> bla(s.begin, s.end);
    std::sort(std::begin(bla), std::end(bla));
    bla.erase(std::unique(std::begin(bla), std::end(bla)), std::end(bla));

    for (size_t i = 0; i < bla.size(); ++i) {
      size_t j = i;
      while (j < bla.size()-1 && bla[j].val+1 == bla[j+1].val && bla[j] != lexer::LAMBDA
	     && isalnum(bla[j].val) && isalnum(bla[j+1].val)) {
	++j;
      }
      if (j-i > 2) {
	os << bla[i] << "-" << bla[j];
	i = j;
      } else {
	os << bla[i];
      }
    }

    return os;
  }

};

template<typename iter_type>
__symbol_writer<iter_type> symbol_writer(iter_type begin, iter_type end) {
  return {begin, end};
}

} //namespace lexer


#endif // LEXER_COMMON_HH_GUARD
