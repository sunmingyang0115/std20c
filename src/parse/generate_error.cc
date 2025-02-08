#include "earley_algorithm.hh"
#include <tuple>

std::string nonTerminalToErrorString(NonTerminals nt) {
    switch (nt) {
    case START: return "start";
    case BSTMTS: return "statements";
    case BSTMT: return "statement";
    case IFCONT: return "if continuation";
    case VARDEF: return "variable definition";
    case EXPR: return "expression";
    case PRE14: return "expression";
    case PRE12: return "expression";
    case PRE11: return "expression";
    case PRE7: return "expression";
    case PRE6: return "expression";
    case PRE4: return "expression";
    case PRE3: return "expression";
    case PRE2: return "expression";
    case PRE1: return "variable id";
    case ARGSOPT: return "arguments";
    case ARGS: return "arguments";
    case LITERAL: return "literal";
    default: return "unknown non-terminal";
    }
}

std::string terminalToErrorString(Terminals t) {
    switch (t) {
        case SPACE: return "space";
        case COMMENT: return "comment";
        case SEMICOLON: return "semicolon";
        case IF: return "if";
        case LPAREN: return "left parenthesis";
        case RPAREN: return "right parenthesis";
        case LBPAREN: return "left brace";
        case RBPAREN: return "right brace";
        case WHILE: return "while";
        case TYPE: return "type";
        case ID: return "identifier";
        case ASSIGN: return "assignment";
        case LOR: return "logical or";
        case LAND: return "logical and";
        case EQ: return "equal";
        case NE: return "not equal";
        case LE: return "less or equal";
        case LT: return "less than";
        case GE: return "greater or equal";
        case GT: return "greater than";
        case PLUS: return "plus";
        case MINUS: return "minus";
        case STAR: return "multiplication";
        case SLASH: return "division";
        case EXCLAIM: return "exclamation";
        case PERIOD: return "period";
        case STRING: return "string literal";
        case NUMBER: return "number literal";
        case COMMA: return "comma";
        case ELSE: return "else";
        default: return "unknown terminal";
    }
}

CompilerError generateParseError(const State &incompleteState, const std::vector<Token> &strippedInput) {
    
    // handles edge case
    auto contextAndBadToken = [&]() -> std::tuple<std::string, Token> {
        if (strippedInput.size() == incompleteState.right) {
            return std::make_tuple("after", std::cref(strippedInput.at(incompleteState.right-1)));
        } else {
            return std::make_tuple("before", std::cref(strippedInput.at(incompleteState.right)));
        }
    }();
    auto [context, badToken] = contextAndBadToken;

    auto msg = [&]() -> std::string {
        if (incompleteState.p.rhs.size() == incompleteState.dot) {
            return "Unidentified token " + context + " " + nonTerminalToErrorString(incompleteState.p.lhs);
        } else {
            auto &e = incompleteState.p.rhs.at(incompleteState.dot);
            if (std::holds_alternative<Terminals>(e)) {
                return "Unexpected token: Expected " + terminalToErrorString(std::get<Terminals>(e)) + " or something similar " + context + " token";
            } else {
                return "Unexpected token: Expected " + nonTerminalToErrorString(std::get<NonTerminals>(e)) + " or something similar " + context + " token";
            }
        }
    }();
    return CompilerError(CompilerError::Type::PARSE, badToken.begin, badToken.length, msg);
            
}