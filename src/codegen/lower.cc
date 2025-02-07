#include "lower.hh"
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <string>
#include <variant>
#include "../vector_util.hh"

std::string generateUniqueLabel() {
    static int n = 0;
    return "__L" + std::to_string(n++);
}

struct LoweringState {
    const SymbolTable &sym;
    IR &ir;
    VReg generateNewReg() {
        auto retReg = this->ir.virtualRegisters.size();
        this->ir.virtualRegisters.insert(retReg);
        return retReg;
    }
    LoweringState(const SymbolTable &sym, IR &ir): sym(sym), ir(ir) {}
};
VReg genExpr(LoweringState &state, const Tree &t);

void genArgs(LoweringState &state, const Tree &t, std::vector<VReg>& regs) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::ARGS));
    if (branch.subtrees.size() == 1) {
        auto reg = genExpr(state, branch.subtrees.at(0));
        regs.push_back(reg);
    } else if (branch.subtrees.size() == 3) {
        auto [expr, _, args] = vectorView<3>(branch.subtrees);
        auto reg = genExpr(state, expr);
        regs.push_back(reg);
        genArgs(state, args, regs);
    } else {
        assert((false));
    }
}

std::vector<VReg> genArgsOpt(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::ARGSOPT));
    std::vector<VReg> regs;
    if (branch.subtrees.size() == 0) return regs;
    genArgs(state, branch.subtrees.at(0), regs);
    return regs;
}


VReg genPre1(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::PRE1));
    
    if (branch.subtrees.size() == 1) {
        auto [id] = vectorView<1>(branch.subtrees);
        auto &idAsToken = std::get<Token>(id);
        auto idReg = state.sym.tokenToVID.at(&idAsToken);
        auto retReg = state.generateNewReg();
        state.ir.instructions.push_back(RegisterAssignInstruction{retReg, idReg});
        return retReg;
    } else {
        auto [id, _1, args, _2] = vectorView<4>(branch.subtrees);

        auto idAsStr = std::get<Token>(id).toString();
        auto retReg = state.generateNewReg();

        std::vector<std::variant<VReg, std::string>> list{idAsStr};
        for (auto e: genArgsOpt(state, args)) {
            list.push_back(e);
        }
        state.ir.instructions.push_back(GenericWriteInstruction(retReg, std::move(list)));
        return retReg;
    }
    assert((false));
}
VReg genLiteral(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::LITERAL));
    auto &token = std::get<Token>(branch.subtrees.at(0));
    std::string s;
    if (token.kind == STRING) {
        s = token.toString().substr(1, token.toString().size() - 2);
    } else {
        s = token.toString();
    }
    auto retReg = state.generateNewReg();
    state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, s});
    return retReg;
}

VReg genPre2(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::PRE2));
    auto [head] = vectorView<1>(branch.subtrees);
    if (std::holds_alternative<Token>(head)) {
        switch (std::get<Token>(head).kind) {
            case LPAREN:
                return genExpr(state, branch.subtrees.at(1));
            case MINUS: {
                auto pre2Reg = genPre2(state, branch.subtrees.at(1));
                auto retReg = state.generateNewReg();
                state.ir.instructions.push_back(GenericWriteInstruction(retReg, {"mul", pre2Reg, "-1"}));
                return retReg;
            }
            case PLUS:
                return genPre2(state, branch.subtrees.at(1));
            case EXCLAIM: {
                auto pre2Reg = genPre2(state, branch.subtrees.at(1));
                auto retReg = state.generateNewReg();
                auto ifTrue = generateUniqueLabel();
                auto end = generateUniqueLabel();
                state.ir.instructions.push_back(GenericReadInstruction({"jmpe", ifTrue, pre2Reg, "0"}));
                state.ir.instructions.push_back(ImmediateAssignInstruction(retReg, "0"));
                state.ir.instructions.push_back(GenericReadInstruction({"jmpe", end, "0", "0"}));
                state.ir.instructions.push_back(GenericReadInstruction({"label", ifTrue}));
                state.ir.instructions.push_back(ImmediateAssignInstruction(retReg, "1"));
                state.ir.instructions.push_back(GenericReadInstruction({"label", end}));
                return retReg;
            }
            default:
                assert((false));
        }
    } else {
        switch(std::get<Branch>(head).nt) {
            case LITERAL:
                return genLiteral(state, head);
            case PRE1:
                return genPre1(state, head);
            default:
                assert(false);
        }
    }
}

VReg genBinOp(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    if (branch.nt == NonTerminals::PRE2) return genPre2(state, t);  // edge case
    assert((branch.nt == NonTerminals::PRE12 
        || branch.nt == NonTerminals::PRE11 
        || branch.nt == NonTerminals::PRE7 
        || branch.nt == NonTerminals::PRE6 
        || branch.nt == NonTerminals::PRE4
        || branch.nt == NonTerminals::PRE3));
    if (branch.subtrees.size() == 1)
        return genBinOp(state, branch.subtrees.at(0));
    auto [left, op, right] = vectorView<3>(branch.subtrees);
    auto retReg = state.generateNewReg();
    if (std::get<Token>(op).kind == LOR || std::get<Token>(op).kind == LAND) {
        // short circuit expression
        if (std::get<Token>(op).kind == LOR) {
            auto returnTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();

            auto leftReg = genBinOp(state, left);
            state.ir.instructions.push_back(GenericReadInstruction{"jmpne", returnTrue, leftReg, "0"});
            auto rightReg = genBinOp(state, right);
            state.ir.instructions.push_back(GenericReadInstruction{"jmpne", returnTrue, rightReg, "0"});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", returnTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
        } else {
            auto returnFalse = generateUniqueLabel();
            auto end = generateUniqueLabel();

            auto leftReg = genBinOp(state, left);
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", returnFalse, leftReg, "0"});
            auto rightReg = genBinOp(state, right);
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", returnFalse, rightReg, "0"});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", returnFalse});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
        }
        return retReg;
    }

    auto leftReg = genBinOp(state, left);
    auto rightReg = genBinOp(state, right);
    
    switch (std::get<Token>(op).kind) {
        case PLUS:
            state.ir.instructions.push_back(GenericWriteInstruction{retReg, {"add", leftReg, rightReg}});
            break;
        case MINUS:
            state.ir.instructions.push_back(GenericWriteInstruction{retReg, {"sub", leftReg, rightReg}});
            break;
        case STAR:
            state.ir.instructions.push_back(GenericWriteInstruction{retReg, {"mul", leftReg, rightReg}});
            break;
        case SLASH:
            state.ir.instructions.push_back(GenericWriteInstruction{retReg, {"div", leftReg, rightReg}});
            break;
        case EQ: {
            auto ifTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", ifTrue, leftReg, rightReg});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", ifTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
            break;
        }
        case NE: {
            auto ifTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();
            state.ir.instructions.push_back(GenericReadInstruction{"jmpne", ifTrue, leftReg, rightReg});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", ifTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
            break;
        } case GT: {
            auto ifTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();
            state.ir.instructions.push_back(GenericReadInstruction{"jmpg", ifTrue, leftReg, rightReg});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", ifTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
            break;
        } case GE: {
            auto ifTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();
            state.ir.instructions.push_back(GenericReadInstruction{"jmpge", ifTrue, leftReg, rightReg});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", ifTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
            break;
        } case LE: {
            auto ifTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();
            state.ir.instructions.push_back(GenericReadInstruction{"jmple", ifTrue, leftReg, rightReg});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", ifTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
            break;
        } case LT: {
            auto ifTrue = generateUniqueLabel();
            auto end = generateUniqueLabel();
            state.ir.instructions.push_back(GenericReadInstruction{"jmpl", ifTrue, leftReg, rightReg});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"jmpe", end, "0", "0"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", ifTrue});
            state.ir.instructions.push_back(ImmediateAssignInstruction{retReg, "1"});
            state.ir.instructions.push_back(GenericReadInstruction{"label", end});
            break;
        }
        default: assert(false);
    }
    return retReg;
}

VReg genPre14(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::PRE14));
    if (branch.subtrees.size() == 3) {
        auto [id, _, pre14] = vectorView<3>(branch.subtrees);
        auto &idAsToken = std::get<Token>(id);
        auto varReg = state.sym.tokenToVID.at(&idAsToken);
        auto pre14Reg = genPre14(state, pre14);
        auto retReg = state.generateNewReg();

        state.ir.instructions.push_back(RegisterAssignInstruction{varReg, pre14Reg});
        state.ir.instructions.push_back(RegisterAssignInstruction{retReg,varReg});
        return retReg;
    } else {
        return genBinOp(state, branch.subtrees.at(0));
    }
}

VReg genExpr(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::EXPR));
    return genPre14(state, branch.subtrees.at(0));
}

void genBStmt(LoweringState &state, const Tree &t);
void genIfCont(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::IFCONT));
    if (branch.subtrees.size() == 0) return;
    auto [_, bstmt] = vectorView<2>(branch.subtrees);
    genBStmt(state, bstmt);
}

void genBStmts(LoweringState &state, const Tree &t);
void genBlock(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::BLOCK));
    return genBStmts(state, branch.subtrees.at(1));
}

void genVardef(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::VARDEF));
    if (branch.subtrees.size() == 2) return;
    auto [_, id, __, expr] = vectorView<4>(branch.subtrees);
    auto &idAsToken = std::get<Token>(id);
    auto varReg = state.sym.tokenToVID.at(&idAsToken);
    auto exprReg = genExpr(state, expr);
    state.ir.instructions.push_back(RegisterAssignInstruction{varReg, exprReg});
}

void genBStmt(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::BSTMT));
    if (std::holds_alternative<Token>(branch.subtrees.at(0))) {
        switch (std::get<Token>(branch.subtrees.at(0)).kind) {
            case IF: {
                auto [_1, _2, expr, _3, bstmt, ifcont] = vectorView<6>(branch.subtrees);
                auto iffalse = generateUniqueLabel();
                auto ifend = generateUniqueLabel();
                auto reg = genExpr(state, expr);
                state.ir.instructions.push_back(GenericReadInstruction({"jmpe", iffalse, reg, "0"}));
                genBStmt(state, bstmt);
                state.ir.instructions.push_back(GenericReadInstruction({"jmpe", ifend, "0", "0"}));
                state.ir.instructions.push_back(GenericReadInstruction({"label", iffalse}));
                genIfCont(state, ifcont);
                state.ir.instructions.push_back(GenericReadInstruction({"label", ifend}));
                break;
            }
            case WHILE: {
                auto [_1, _2, expr, _3, bstmt] = vectorView<5>(branch.subtrees);
                auto beginning = generateUniqueLabel();
                auto iffalse = generateUniqueLabel();
                state.ir.instructions.push_back(GenericReadInstruction({"label", beginning}));
                auto reg = genExpr(state, expr);
                state.ir.instructions.push_back(GenericReadInstruction({"jmpe", iffalse, reg, "0"}));
                genBStmt(state, bstmt);
                state.ir.instructions.push_back(GenericReadInstruction({"jmpe", beginning, "0", "0"}));
                state.ir.instructions.push_back(GenericReadInstruction({"label", iffalse}));
                break;
            }
            default: assert((false));
        }
    } else {
        switch (std::get<Branch>(branch.subtrees.at(0)).nt) {
            case VARDEF:
                genVardef(state, branch.subtrees.at(0));
                break;
            case EXPR:
                genExpr(state, branch.subtrees.at(0));
                break;
            case BLOCK:
                genBlock(state, branch.subtrees.at(0));
                break;
            default:
                assert((false));
        }
    }
}

void genBStmts(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::BSTMTS));
    if (branch.subtrees.size() == 2) {
        auto [bstmts, bstmt] = vectorView<2>(branch.subtrees);
        genBStmts(state, bstmts);
        genBStmt(state, bstmt);
    }
}

void genStart(LoweringState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((branch.nt == NonTerminals::START));
    genBStmts(state, branch.subtrees.at(0));
}

IR generateIR(const SymbolTable &sym, const Tree &t) {
    IR ir;
    LoweringState state(sym, ir);
    // add varids to virtual regs
    for (auto &[a, b]: state.sym.vidToType) {
        ir.virtualRegisters.insert(a);
    }
    genStart(state, t);
    return ir;
}
