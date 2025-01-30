#ifndef LANGUAGE_HH
#define LANGUAGE_HH
#include <cstddef>
#include <initializer_list>
#include <variant>
#include <vector>
#include <regex>

enum Terminals {
    SPACE, COMMENT, SEMICOLON, IF, ELSE, LPAREN, RPAREN, LBPAREN, RBPAREN, WHILE, TYPE, ID, ASSIGN, LOR, LAND, EQ, NE, LE, LT, GE, GT, PLUS, MINUS, STAR, SLASH, EXCLAIM, PERIOD, NUMBER, STRING, COMMA
};
enum NonTerminals {
    START, BSTMTS, BSTMT, BLOCK, IFCONT, VARDEF, EXPR, PRE14, PRE12, PRE11, PRE7, PRE6, PRE4, PRE3, PRE2, PRE1, ARGSOPT, ARGS, LITERAL
};
struct ProductionRule {
    NonTerminals lhs;
    std::vector<std::variant<Terminals, NonTerminals>> rhs;
};
struct KindToRegex {
    Terminals kind;
    std::regex regex;
    KindToRegex(Terminals kind, std::string regex): kind(kind), regex(regex) {}
};
struct Token {
    std::string::const_iterator begin;
    std::size_t length; // length of the lexeme
    Terminals kind;
    Token(std::string::const_iterator begin, std::size_t length, Terminals kind): begin(begin), length(length), kind(kind) {}
    std::string toString() const {
        return std::string(begin, begin + length);
    }
};
const std::vector<KindToRegex> tokenizationRules {
    {SPACE, "^\\s+"},
    {COMMENT, "^\\/\\/.+"},
    {SEMICOLON, "^;"},
    {IF, "^if"},
    {ELSE, "^else"},
    {LPAREN, "^\\("},
    {RPAREN, "^\\)"},
    {LBPAREN, "^\\{"},
    {RBPAREN, "^\\}"},
    {WHILE, "^while"},
    {TYPE, "(^Entity)|(^Number)|(^Vector)|(^String)"},
    {NUMBER, "^[-+]?\\d+(\\.\\d+)?"},
    {STRING, "^\".*\""},
    {ID, "^[a-zA-Z_][a-zA-Z0-9_]*"},
    {ASSIGN, "^="},
    {LOR, "^\\|\\|"},
    {LAND, "^\\&\\&"},
    {EQ, "^=="},
    {NE, "^!="},
    {LE, "^<="},
    {LT, "^<"},
    {GE, "^>="},
    {GT, "^>"},
    {PLUS, "^\\+"},
    {MINUS, "^-"},
    {STAR, "^\\*"},
    {SLASH, "^\\/"},
    {EXCLAIM, "^!"},
    {PERIOD, "^\\."},
    {COMMA, "^\\,"}
};

const std::vector<ProductionRule> grammar {
    {START, {BSTMTS}},
    {BSTMTS, {BSTMTS, BSTMT}},
    {BSTMTS, {}},
    {BSTMT, {VARDEF, SEMICOLON}},
    {BSTMT, {EXPR, SEMICOLON}},
    {BSTMT, {BLOCK}},
    {BSTMT, {IF, LPAREN, EXPR, RPAREN, BSTMT, IFCONT}},
    {BSTMT, {WHILE, LPAREN, EXPR, RPAREN, BSTMT}},
    {BLOCK, {LBPAREN, BSTMTS, RBPAREN}},
    {IFCONT, {ELSE, BSTMT}},
    {IFCONT, {}},
    {VARDEF, {TYPE, ID}},
    {VARDEF, {TYPE, ID, ASSIGN, EXPR}},
    {EXPR, {PRE14}},
    {PRE14, {ID, ASSIGN, PRE14}},
    {PRE14, {PRE12}},
    {PRE12, {PRE12, LOR, PRE11}},
    {PRE12, {PRE11}},
    {PRE11, {PRE11, LAND, PRE7}},
    {PRE11, {PRE7}},
    {PRE7, {PRE7, EQ, PRE6}},
    {PRE7, {PRE7, NE, PRE6}},
    {PRE7, {PRE6}},
    {PRE6, {PRE6, LE, PRE4}},
    {PRE6, {PRE6, LT, PRE4}},
    {PRE6, {PRE6, GE, PRE4}},
    {PRE6, {PRE6, GT, PRE4}},
    {PRE6, {PRE4}},
    {PRE4, {PRE4, PLUS, PRE3}},
    {PRE4, {PRE4, MINUS, PRE3}},
    {PRE4, {PRE3}},
    {PRE3, {PRE3, STAR, PRE2}},
    {PRE3, {PRE3, SLASH, PRE2}},
    {PRE3, {PRE2}},
    {PRE2, {LPAREN, EXPR, RPAREN}},
    {PRE2, {MINUS, PRE2}},
    {PRE2, {PLUS, PRE2}},
    {PRE2, {EXCLAIM, PRE2}},
    {PRE2, {PRE1}},
    {PRE2, {LITERAL}},  // literals cannot preform fn calls and member reference
    {LITERAL, {STRING}},
    {LITERAL, {NUMBER}},
    {PRE1, {ID}},
    {PRE1, {ID, LPAREN, ARGSOPT, RPAREN}},    // no closures, so [0] = ID instead of PRE1
    // {PRE1, {PRE1, PERIOD, PRE1}},    add this later
    {ARGSOPT, {}},
    {ARGSOPT, {ARGS}},
    {ARGS, {EXPR}},
    {ARGS, {EXPR, COMMA, ARGS}},
};

struct Branch;
using Tree = std::variant<Branch, Token>;
struct Branch {
    NonTerminals nt;
    std::size_t left, right;
    std::vector<Tree> subtrees;
    Branch(NonTerminals nt, std::size_t left, std::size_t right, std::vector<Tree> subtrees): nt(nt), left(left), right(right), subtrees(subtrees) {}
    ~Branch() = default;
};


#endif
