#include "emit_table.hh"
#include "DFA.hh"
#include "parser.hh"

#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include "time.h"

using namespace lexer;

void lexer::table_emitter::emit_dfa(
  const DFA & d, 
  std::vector<tkn_rule> &tkn_rules,
  const std::string &outputDirectory) {

  std::string hhFilename = outputDirectory + "table.hh";
  std::string ccFilename = outputDirectory + "table.cc";

  std::ofstream hhFile(hhFilename);
  std::ofstream ccFile(ccFilename);

  if (hhFile.fail()) {
    throw std::runtime_error("Could not open: " + hhFilename);
  }
  if (ccFile.fail()) {
    throw std::runtime_error("Could not open: " + ccFilename);
  }

  std::vector<std::string> names;
  for (auto const & r: tkn_rules)
    names.push_back(r.name);

  auto delta = d.getDelta();
  auto accepts = d.getAcceptStates();
  size_t numberOfStates = d.getNumberOfStates();
  state q0 = d.getInitialState();
  size_t INVALID=numberOfStates;
  
  state rejectState;
  for (state s = 0; s < numberOfStates; ++s) {
    if (accepts.count(s)) continue;
    auto iter = delta.lower_bound({s, symbol::min() });
    while (iter->first.first == s) {
      if (iter->second != s) break;
      ++iter;
    }
    if (iter->first.first == s) continue;
    rejectState = s;
    break;
  }
  
  std::stringstream oscc;
  std::stringstream oshh;
  hhFile << "#ifndef TABLE_HH_GUARD" << std::endl;
  hhFile << "#define TABLE_HH_GUARD" << std::endl << std::endl;

  hhFile << "#include <iostream>" << std::endl;
  hhFile << "#include <stdint.h>" << std::endl << std::endl;

  hhFile << "namespace lexer {" << std::endl << std::endl;

  hhFile << "enum class TableTokenType {" << std::endl;
  hhFile << "    INVALID=" << INVALID;
  size_t i=INVALID+1;
  for (auto &s : names) 
    hhFile << "," << std::endl << "    " << s << "=" << i++;
  hhFile << "};" << std::endl;
  hhFile << "extern int table[][256];" << std::endl;
  hhFile << "const int initialState=" << q0 << ";" << std::endl;
  hhFile << "} // end namespace lexer" << std::endl << std::endl;
  hhFile << "#endif // TABLE_HH_GUARD" << std::endl;
  
  ccFile << "#include \"table.hh\"" << std::endl << std::endl;
  ccFile << "namespace lexer {" << std::endl << std::endl;
  ccFile << "int table[][256] = {";

  bool first=true;
  for (state s = 0; s < numberOfStates; ++s) {
    int jumps[256];
    if (accepts.count(s)) 
      std::fill(jumps, jumps+256, INVALID+accepts[s]);
    else
      std::fill(jumps, jumps+256, INVALID);
    for (auto iter = delta.lower_bound({s, symbol::min() }); 
	 iter->first.first == s; 
	 ++iter) {
      if (iter->second == rejectState) continue;
      jumps[iter->first.second.val] = iter->second;
    }
    if (first) first=false;
    else ccFile << ',';
    ccFile << std::endl << "    {";
    for (int i=0; i < 256; ++i) {
      if (i != 0) ccFile << ", ";
      ccFile << jumps[i];
    }
    ccFile << "}";
  }
  ccFile << std::endl << "};" << std::endl;
  ccFile << "} // end namespace lexer" << std::endl;
}
