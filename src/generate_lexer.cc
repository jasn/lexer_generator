#include "emit_c++.hh"
#include "emit_table.hh"
#include "parser.hh"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace lexer;

void printUsage(std::ostream & o) {
  o << "The program generates a fast lexer." << std::endl << std::endl;
  o << "Usage: ./generate_lexer [--emit-cpp] [--emit-table] <regexp_file> <output_directory>" << std::endl << std::endl;

  o << "The <regexp_file> is a collection of token definitions on the form:" << std::endl << std::endl;
  o << "<TOKEN_NAME> := <regexp definition>" << std::endl << std::endl;
  o << "Each definition is on its own line." << std::endl;
  o << "The syntax for the regular expression is as follows." << std::endl;
  o << "A single character is a regular expression: c" << std::endl;
  o << "The special character '.' matches any one single character." << std::endl;
  o << "The regular expression <regexp_1>|<regexp_2> matches either <regexp_1> or <regexp_2>." << std::endl;
  o << "The regular expression <regexp>* matches <regexp> 0 or more times (Kleene's star)." << std::endl;
  o << "The regular expression <regexp>+ matches <regexp> 1 or more times." 
	    << std::endl << std::endl;
  
  o << "Character ranges are also supported: [a-z] matches any single character in the range a to z." << std::endl;
  o << "One bracket can contain several ranges: [a-zA-Z0-9]." << std::endl;
  o << "Brackets can also be used as negation, i.e. match anything that is not in the range." << std::endl
	    << "For example [^0-9a-z] matches any character except 0 to 9 and a-z." << std::endl << std::endl;

  o << "In the <output_directory> two files will be created: tokenizer.hh and tokenizer.cc." << std::endl << "These two files make up the lexer." << std::endl;
  o << "Currently only C++11 lexers are supported, but more languages can be added." << std::endl
    << "Look at 'generate_lexer.cc', 'emit_c++.hh', and 'emit_c++.cc' for adding new languages." << std::endl;
}

int main(int argc, char *argv[]) {
  bool emit_cpp=false;
  bool emit_table=false;
  bool show_usage=false;

  std::vector<std::string> positional;
  for (char ** arg = argv+1; *arg; ++arg) {
    std::string a=*arg;
    if (a[0] != '-') { 
      positional.push_back(a);
      continue;
    }
    if (a[1] == '-') {
      if (a == "--help") 
	show_usage=true;
      else if (a == "--emit-cpp")
	emit_cpp=true;
      else if (a == "--emit-table")
	emit_table=true;
      else {
	std::cerr << "Unknwon switch " << a << std::endl;
	printUsage(std::cerr);
	return EXIT_FAILURE;
      }
      continue;
    }
    for (const char * c=a.data()+1; *c; ++c) {
      switch(*c) {
      case 'h':
	show_usage=true;
	break;
      default:
	std::cerr << "Unknwon switch " << *c << std::endl;
	printUsage(std::cerr);
	return EXIT_FAILURE;
      }
    }
  }

  if (positional.size() < 1) {
    std::cerr << "Must specify input file" << std::endl;
    printUsage(std::cerr);
    return EXIT_FAILURE;    
  }

  if (positional.size() > 2) {
    std::cerr << "Unexpected thrid argument" << std::endl;
    printUsage(std::cerr);
    return EXIT_FAILURE;    
  }

  if (show_usage) {
    printUsage(std::cout);
    return EXIT_SUCCESS;
  }
  
  std::string tokenFile(positional[0]);
  std::string outputDirectory="./";
  if (positional.size() > 1) 
    outputDirectory = positional[1];

  std::cout << "Reading input" << std::endl;
  std::fstream fs(tokenFile);
  std::vector<tkn_rule> tkn_rules = std::move(parseFile(fs));
  
  NFA f(1, std::unordered_map<state, acceptType>(), 0, {});
  for (size_t i = 0; i < tkn_rules.size(); ++i)
    f = lexer::NFA::join(f, tkn_rules[i].regexp->getNFA(i+1));
  std::cout << "Determinizing" << std::endl;
  DFA d = f.determinize();
  std::cout << "Minimizing" << std::endl;
  d.minimize();

  if (emit_cpp) {
    std::cout << "Outputting c++" << std::endl;
    cpp_emitter::emit_dfa(d, tkn_rules,  outputDirectory);
  }

  if (emit_table) {
    std::cout << "Outputting table" << std::endl;
    table_emitter::emit_dfa(d, tkn_rules,  outputDirectory);
  }

  std::cout << "Done" << std::endl;
}
