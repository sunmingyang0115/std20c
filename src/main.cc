#include "error_message.hh"
#include "language.hh"
#include "scan/tokenize.hh"
#include "parse/parser.hh"
#include "analysis/semantics.hh"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <variant>
#include <vector>
#include "error_message.hh"

std::ostream& operator<<(std::ostream& os, Terminals t) {
    switch (t) {
        case SPACE: return os << "SPACE";
        case COMMENT: return os << "COMMENT";
        case SEMICOLON: return os << "SEMICOLON";
        case IF: return os << "IF";
        case LPAREN: return os << "LPAREN";
        case RPAREN: return os << "RPAREN";
        case LBPAREN: return os << "LBPAREN";
        case RBPAREN: return os << "RBPAREN";
        case WHILE: return os << "WHILE";
        case TYPE: return os << "TYPE";
        case ID: return os << "ID";
        case ASSIGN: return os << "ASSIGN";
        case LOR: return os << "LOR";
        case LAND: return os << "LAND";
        case EQ: return os << "EQ";
        case NE: return os << "NE";
        case LE: return os << "LE";
        case LT: return os << "LT";
        case GE: return os << "GE";
        case GT: return os << "GT";
        case PLUS: return os << "PLUS";
        case MINUS: return os << "MINUS";
        case STAR: return os << "STAR";
        case SLASH: return os << "SLASH";
        case EXCLAIM: return os << "EXCLAIM";
        case PERIOD: return os << "PERIOD";
        case LITERAL: return os << "LITERAL";
        case COMMA: return os << "COMMA";
        default: return os << "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, NonTerminals nt) {
    switch (nt) {
        case START: return os << "START";
        case BSTMTS: return os << "BSTMTS";
        case CONTROL: return os << "CONTROL";
        case BSTMT: return os << "BSTMT";
        case VARDEF: return os << "VARDEF";
        case EXPR: return os << "EXPR";
        case PRE14: return os << "PRE14";
        case PRE12: return os << "PRE12";
        case PRE11: return os << "PRE11";
        case PRE7: return os << "PRE7";
        case PRE6: return os << "PRE6";
        case PRE4: return os << "PRE4";
        case PRE3: return os << "PRE3";
        case PRE2: return os << "PRE2";
        case PRE1: return os << "PRE1";
        case ARGSOPT: return os << "ARGSOPT";
        case ARGS: return os << "ARGS";
        default: return os << "UNKNOWN";
    }
}

void printTree(std::ostream& o, const Tree &t, std::size_t depth = 0) {
    std::string indent(depth * 2, ' ');
    if (std::holds_alternative<Token>(t)) {
        auto token = std::get<Token>(t);

        o << indent << token.kind << " " << token.lexeme << "\n";
    } else if (std::holds_alternative<Branch>(t)) {
        const auto &branch = std::get<Branch>(t);
        o << indent << "(" << branch.nt << "\n";
        for (const auto &subtree : branch.subtrees) {
            printTree(o, subtree, depth + 1);
        }
        o << indent << ")\n";
    }
}

int main(int argc, char* argv[]) {
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }
    std::string s((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto scan = maximalMunch(s);
    if (std::holds_alternative<CompilerError>(scan)) {
        return generateMessage(s, std::get<CompilerError>(scan));
    } 
    auto tokens = std::get<std::vector<Token>>(scan);
    // for (auto &e: tokens) {
    //     std::cout << "{" << e.kind << ", " << e.lexeme << "}";   
    // }
    auto parse = earleyParser(tokens);
    // if (auto *error = std::get_if())
    
    if (std::holds_alternative<CompilerError>(parse)) {
        return generateMessage(s, std::get<CompilerError>(parse));
    }
    printTree(std::cout, std::get<Tree>(parse));
}