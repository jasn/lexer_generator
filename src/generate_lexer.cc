#include "emit_c++.hh"
#include "parser.hh"

#include <iostream>
#include <fstream>

#include <cstdlib>

using namespace lexer;

void printUsage() {
  std::cout << "The program generates a fast lexer." << std::endl << std::endl;
  std::cout << "Usage: ./generate_lexer <regexp_file> <output_directory>" << std::endl << std::endl;

  std::cout << "The <regexp_file> is a collection of token definitions on the form:" << std::endl << std::endl;
  std::cout << "<TOKEN_NAME> := <regexp definition>" << std::endl << std::endl;
  std::cout << "Each definition is on its own line." << std::endl;
  std::cout << "The syntax for the regular expression is as follows." << std::endl;
  std::cout << "A single character is a regular expression: c" << std::endl;
  std::cout << "The special character '.' matches any one single character." << std::endl;
  std::cout << "The regular expression <regexp_1>|<regexp_2> matches either <regexp_1> or <regexp_2>." << std::endl;
  std::cout << "The regular expression <regexp>* matches <regexp> 0 or more times (Kleene's star)." << std::endl;
  std::cout << "The regular expression <regexp>+ matches <regexp> 1 or more times." 
	    << std::endl << std::endl;
  
  std::cout << "Character ranges are also supported: [a-z] matches any single character in the range a to z." << std::endl;
  std::cout << "One bracket can contain several ranges: [a-zA-Z0-9]." << std::endl;
  std::cout << "Brackets can also be used as negation, i.e. match anything that is not in the range." << std::endl
	    << "For example [^0-9a-z] matches any character except 0 to 9 and a-z." << std::endl << std::endl;

  std::cout << "In the <output_directory> two files will be created: tokenizer.hh and tokenizer.cc." << std::endl << "These two files make up the lexer." << std::endl;
  std::cout << "Currently only C++11 lexers are supported, but more languages can be added." << std::endl
	    << "Look at 'generate_lexer.cc', 'emit_c++.hh', and 'emit_c++.cc' for adding new languages." << std::endl;
    
  return;
}

// templated on emitter
template<typename E>
void emit(const std::string &tokenFile, const std::string &outputDirectory) {

  std::fstream fs(tokenFile);
  std::vector<tkn_rule> tkn_rules = std::move(parseFile(fs));

  E::emit_dfa(tkn_rules, outputDirectory);
  
}

int main(int argc, char *argv[]) {
  
  if (argc != 3) {
    printUsage();
    exit(EXIT_FAILURE);
  }

  std::string tokenFile(argv[1]);
  std::string outputDirectory(argv[2]);
  
  emit<cpp_emitter>(tokenFile, outputDirectory);


}
