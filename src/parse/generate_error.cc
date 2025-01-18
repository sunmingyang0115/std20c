#include "earley_algorithm.hh"
#include <iostream>
#include <tuple>

std::string nonTerminalToErrorString(NonTerminals nt) {
    switch (nt) {
    case START: return "start";
    case BSTMTS: return "statements";
    case BSTMT: return "statement";
    case CONTROL: return "control";
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
        case LITERAL: return "literal";
        case COMMA: return "comma";
        default: return "unknown terminal";
    }
}

CompilerError generateParseError(const State &lastValidState, const std::vector<Token> &originalInput) {
    auto [stringPos, stringLength] = [&](size_t strippedTokenPos) {
        // first convert strippedTokenPos to tokenPos
        size_t tokenPos = 0;
        size_t i = 0;
        for (auto &token: originalInput) {
            
            if (!skipTokenPredicate(token)) {
                if (i == strippedTokenPos) {
                    break;
                }
                i++;
            }
            tokenPos++;
        }
        size_t stringPos = 0;
        // now we convert tokenPos to stringPos
        for (size_t i = 0; i < tokenPos; i++) {
            stringPos += originalInput[i].lexeme.size();
        }
        // std::cerr << strippedTokenPos << " " << tokenPos << " " << stringPos << "\n";
        return std::make_tuple(stringPos, originalInput[tokenPos].lexeme.size());
    }(lastValidState.right);
    auto msg = [&]() -> std::string {
        if (lastValidState.p.rhs.size() == lastValidState.dot) {
            return "Unidentified token after " + nonTerminalToErrorString(lastValidState.p.lhs);
        } else {
            auto &e = lastValidState.p.rhs[lastValidState.dot];
            if (std::holds_alternative<Terminals>(e)) {
                return "Unexpected token: Expected " + terminalToErrorString(std::get<Terminals>(e)) + " symbol or something similar";
            } else {
                return "Unexpected token: Expected " + nonTerminalToErrorString(std::get<NonTerminals>(e)) + " or something similar";
            }
        }
    }();
    return CompilerError(CompilerError::Type::Parse, stringPos, stringLength, msg);
            
}