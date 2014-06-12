#ifndef LEXER_COMMON_HH_GUARD
#define LEXER_COMMON_HH_GUARD

#include <stdint.h>

namespace lexer {

  typedef uint32_t state;
  typedef uint8_t symbol;
  typedef uint32_t acceptType;
  
  const acceptType REJECT = 0;

  const symbol LAMBDA = 31;

}

#endif // LEXER_COMMON_HH_GUARD
