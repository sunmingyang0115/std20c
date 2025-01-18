#ifndef PARSER_HH
#define PARSER_HH
#include <optional>
#include <variant>
#include <vector>
#include "../language.hh"

std::optional<Tree> earleyParser(const std::vector<Token> &input);

#endif