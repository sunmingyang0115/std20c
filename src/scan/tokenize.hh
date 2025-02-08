#ifndef TOKENIZE_HH
#define TOKENIZE_HH
#include <std20c/language.hh>
#include <std20c/error_message.hh>

std::variant<CompilerError, std::vector<Token>> maximalMunch(const std::string &s);

#endif