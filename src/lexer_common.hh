#ifndef LEXER_COMMON_HH_GUARD
#define LEXER_COMMON_HH_GUARD

#include <stdint.h>
#include <set>
#include <unordered_set>

namespace lexer {

  typedef uint32_t state;
  typedef uint8_t symbol;
  typedef uint32_t acceptType;
  
  const acceptType REJECT = 0;

  const symbol LAMBDA = 31;

  template <typename T>
  const std::unordered_set<symbol> getAlphabet(const T &FA) {
    
    std::unordered_set<symbol> res;
    for (auto x : FA.getDelta()) {
      res.insert(x.first.second);
    }
    return res;
  }


}

#endif // LEXER_COMMON_HH_GUARD
