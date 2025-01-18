#ifndef LANGUAGE_HH
#define LANGUAGE_HH
#include <variant>
#include <vector>
#include <regex>

enum Terminals {
    SPACE, COMMENT, SEMICOLON, IF, LPAREN, RPAREN, LBPAREN, RBPAREN, WHILE, TYPE, ID, ASSIGN, LOR, LAND, EQ, NE, LE, LT, GE, GT, PLUS, MINUS, STAR, SLASH, EXCLAIM, PERIOD, LITERAL, COMMA
};
enum NonTerminals {
    START, BSTMTS, BSTMT, CONTROL, VARDEF, EXPR, PRE14, PRE12, PRE11, PRE7, PRE6, PRE4, PRE3, PRE2, PRE1, ARGSOPT, ARGS
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
    std::string lexeme;
    Terminals kind;
    Token(std::string lexeme, Terminals kind): lexeme(lexeme), kind(kind) {}
};
const std::vector<KindToRegex> tokenizationRules {
    {SPACE, "^\\s+"},
    {COMMENT, "^\\/\\/.+"},
    {SEMICOLON, "^;"},
    {IF, "^if"},
    {LPAREN, "^\\("},
    {RPAREN, "^\\)"},
    {LBPAREN, "^\\{"},
    {RBPAREN, "^\\}"},
    {WHILE, "^while"},
    {TYPE, "(^Entity)|(^Number)|(^Vector)|(^String)"},
    {LITERAL, "^[-+]?\\d+(\\.\\d+)?"},
    {ID, "^\\w+"},
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
    
    {COMMA, "^\\/\\/.+"}
};

const std::vector<ProductionRule> grammar {
    {START, {BSTMTS}},
    {BSTMTS, {BSTMTS, BSTMT, SEMICOLON}},
    {BSTMTS, {BSTMTS, CONTROL}},
    {BSTMTS, {}},
    {BSTMT, {VARDEF}},
    {BSTMT, {EXPR}},
    {CONTROL, {IF, LPAREN, EXPR, RPAREN, LBPAREN, BSTMTS, RBPAREN}},
    {CONTROL, {WHILE, LPAREN, EXPR, RPAREN, LBPAREN, BSTMTS, RBPAREN}},
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
    {PRE2, {MINUS, PRE2}},
    {PRE2, {PLUS, PRE2}},
    {PRE2, {EXCLAIM, PRE2}},
    {PRE2, {PRE1}},
    {PRE2, {LITERAL}},  // literals cannot preform fn calls and member reference
    {PRE1, {ID}},
    {PRE1, {PRE1, LPAREN, ARGSOPT, RPAREN}},
    {PRE1, {PRE1, PERIOD, PRE1}},
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
