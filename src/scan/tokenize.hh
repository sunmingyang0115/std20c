#ifndef TOKENIZE_H
#define TOKENIZE_H
#include "../language.hh"
#include "../error_message.hh"

std::variant<CompilerError, std::vector<Token>> maximalMunch(std::string s);

#endif