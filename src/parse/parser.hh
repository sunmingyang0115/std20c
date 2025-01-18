#ifndef PARSER_HH
#define PARSER_HH
#include <variant>
#include <vector>
#include "../language.hh"
#include "../error_message.hh"

std::variant<CompilerError, Tree> earleyParser(const std::vector<Token> &input);

#endif