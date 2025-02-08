#ifndef PARSER_HH
#define PARSER_HH
#include <std20c/language.hh>
#include <std20c/error_message.hh>

std::variant<CompilerError, Tree> earleyParser(const std::vector<Token> &input);

#endif