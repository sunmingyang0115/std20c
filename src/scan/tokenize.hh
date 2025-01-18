#ifndef TOKENIZE_HH
#define TOKENIZE_HH
#include "../language.hh"
#include "../error_message.hh"

std::variant<CompilerError, std::vector<Token>> maximalMunch(std::string s);

#endif