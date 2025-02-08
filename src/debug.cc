#include "debug.hh"
#include "std20c/compilation.hh"
#include <cassert>
#include <ostream>

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
        case NUMBER: return os << "NUMBER";
        case STRING: return os << "STRING";
        case COMMA: return os << "COMMA";
        default: return os << "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, NonTerminals nt) {
    switch (nt) {
        case START: return os << "START";
        case BSTMTS: return os << "BSTMTS";
        case IFCONT: return os << "IFCONT";
        case BLOCK: return os << "BLOCK";
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
        case LITERAL: return os << "LITERAL";
        case ARGSOPT: return os << "ARGSOPT";
        case ARGS: return os << "ARGS";
        default: return os << "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, Type t) {
    switch (t) {
        case STRING_TYPE: return os << "STRING_TYPE";
        case NUMBER_TYPE: return os << "NUMBER_TYPE";
        case ENTITY_TYPE: return os << "ENTITY_TYPE";
        case VECTOR_TYPE: return os << "VECTOR_TYPE";
        case VOID_TYPE: return os << "VOID_TYPE";
        default: return os << "UNKNOWN_TYPE";
        }
}

void printTree(std::ostream& o, const Tree &t, std::size_t depth = 0) {
    std::string indent(depth * 2, ' ');
    if (std::holds_alternative<Token>(t)) {
        auto token = std::get<Token>(t);

        o << indent << token.kind << " ";
        auto it = token.begin;
        for (size_t i = 0; i < token.length; i++) {
            o << *it;
            it++;
        }
        o << "\n";
    } else if (std::holds_alternative<Branch>(t)) {
        const auto &branch = std::get<Branch>(t);
        o << indent << "(" << branch.nt << "\n";
        for (const auto &subtree : branch.subtrees) {
            printTree(o, subtree, depth + 1);
        }
        o << indent << ")\n";
    }
}
std::ostream& operator<<(std::ostream& os, const Tree &t) {
    printTree(os, t);
    return os;
}

std::ostream& operator<<(std::ostream &os, const IR &ir) {
    os << "#lang std20\n";
    for (auto &e: ir.instructions) {
        if (std::holds_alternative<GenericWriteInstruction>(e)) {
            auto &assign = std::get<GenericWriteInstruction>(e);
            os << "$" << assign.lhs << " = ";
            for (auto &rhs_e: assign.rhs) {
                if (std::holds_alternative<VReg>(rhs_e)) {
                    auto &vreg = std::get<VReg>(rhs_e);
                    os << "$" << vreg << " ";
                } else {
                    auto &str = std::get<std::string>(rhs_e);
                    os << str << " ";
                }
            }
           os << "\n";
        } else if (std::holds_alternative<GenericReadInstruction>(e)) {
            auto &op = std::get<GenericReadInstruction>(e);
            for (auto &rhs_e: op.instruction) {
                if (std::holds_alternative<VReg>(rhs_e)) {
                    auto &vreg = std::get<VReg>(rhs_e);
                    os << "$" << vreg << " ";
                } else {
                    auto &str = std::get<std::string>(rhs_e);
                    os << str << " ";
                }
            }
            os << "\n";
        } else if (std::holds_alternative<ImmediateAssignInstruction>(e)) {
            auto &ins = std::get<ImmediateAssignInstruction>(e);
            os << "$" << ins.lhs << " = mov " << ins.value << "\n";
        } else if (std::holds_alternative<RegisterAssignInstruction>(e)) {
            auto &ins = std::get<RegisterAssignInstruction>(e);
            os << "$" << ins.lhs << " = mov $" << ins.rhs << "\n";
        } else {
            assert((false));
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream &os, const SymbolTable &semantics) {
    for (auto &[a, b]: semantics.tokenToVID) {
        os << a << " ::= " << b << "\n";
    }
    for (auto &[a, b]: semantics.vidToType) {
        os << a << " ::= " << b << "\n";
    }
    for (auto &[a, b]: semantics.funNameToType) {
        os << a << "\n";
    }
    return os;   
}