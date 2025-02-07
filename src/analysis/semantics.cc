#include "semantics.hh"
#include <cassert>
#include "../vector_util.hh"
#include "scope.hh"
#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>

Type tokenToType(Token t) {
    auto s = t.toString();
    if (s == "Number") return Type::NUMBER_TYPE;
    if (s == "String") return Type::STRING_TYPE;
    if (s == "Vector") return Type::VECTOR_TYPE;
    if (s == "Entity") return Type::ENTITY_TYPE;
    throw std::runtime_error("Unknown Type");
}

struct SemanticState {
    SymbolTable &symbolTable;
    VariableScopeContext context{};

    std::optional<CompilerError> error{std::nullopt};
    SemanticState(SymbolTable &symbolTable): symbolTable(symbolTable) {};
};

std::optional<Type> genExpr(SemanticState &state, const Tree &t);

bool genArgs(SemanticState &state, const Tree &t, std::vector<Type>& types) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::ARGS));
    if (branch.subtrees.size() == 1) {
        if (auto type = genExpr(state, branch.subtrees.at(0))) {
            if (!type.has_value()) return false;
            types.push_back(*type);
        }
        return true;
    } else if (branch.subtrees.size() == 3) {
        auto [expr, _, args] = vectorView<3>(branch.subtrees);
        if (auto type = genExpr(state, expr)) {
            if (!type.has_value()) return false;
            types.push_back(*type);
        }
        return genArgs(state, args, types);
    } else {
        assert((false));
    }
}


std::optional<std::vector<Type>> genArgsOpt(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::ARGSOPT));
    std::vector<Type> types;
    if (branch.subtrees.size() == 0 || genArgs(state, branch.subtrees.at(0), types)) {
        return types;
    } else {
        return std::nullopt;
    }
}

std::optional<Type> genID(SemanticState &state, const Tree &t)  {
    auto &token = std::get<Token>(t);
    assert((!state.error && token.kind == Terminals::ID));
    auto idAsStr = token.toString();
    // std::cerr << "aming";
    if (auto vid = state.context.searchAllScopes(idAsStr)) {
        state.symbolTable.tokenToVID.emplace(&token, *vid);
        return state.symbolTable.vidToType[*vid];
    } else {
        state.error = undefinedVariableError(token);
        return std::nullopt;
    }
}
std::optional<Type> genLiteral(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::LITERAL));
    switch (std::get<Token>(branch.subtrees.at(0)).kind) {
        case Terminals::NUMBER:
        return Type::NUMBER_TYPE;
        case Terminals::STRING:
        return Type::STRING_TYPE;
        default:
        assert((false));
    }
}

std::optional<Type> genPre1(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::PRE1));
    if (branch.subtrees.size() == 1) {
        
        return genID(state, branch.subtrees.at(0));
    } else {
        // functions
        auto [fun, _, args, __] = vectorView<4>(branch.subtrees);
        auto &funAsToken = std::get<Token>(fun);
        auto funAsStr = funAsToken.toString();
        auto it = state.symbolTable.funNameToType.find(funAsStr);
        if (it != state.symbolTable.funNameToType.end()) {
            auto &funAsType = it->second;
            auto argTypes = genArgsOpt(state, args);

            auto compareArgs = [&](std::vector<Type> v1, std::vector<Type> v2) {
                if (v1.size() != v2.size()) return false;
                for (size_t i = 0; i < v1.size(); i++) {
                    if (v1[i] == Type::OBJECT_TYPE || v2[i] == Type::OBJECT_TYPE) continue;
                    if (v1[i] == v2[i]) continue;
                    return false;
                }
                return true;
            };

            if (argTypes.has_value() && compareArgs(*argTypes, funAsType.args)) {
                return funAsType.ret;
            } else {
                if (argTypes.has_value()) {
                    // type mismatch
                    state.error = invalidArgumentError(funAsToken, funAsType.args, *argTypes);
                }
                return std::nullopt;
            }
        } else {
            // function not defined
            state.error = undefinedFunctionError(funAsToken);
            return std::nullopt;
        }

    }
}

std::optional<Type> genPre2(SemanticState &state, const Tree &t) {
    
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::PRE2));
    if (branch.subtrees.size() == 1) {
        auto &child = branch.subtrees.at(0);
        switch (std::get<Branch>(child).nt) {
            case NonTerminals::LITERAL:
                return genLiteral(state, child);
            case NonTerminals::PRE1:
                return genPre1(state, child);
            default:
            assert((false));
        }
    } else if (branch.subtrees.size() == 3) {
        auto [_, expr, __] = vectorView<3>(branch.subtrees);
        return genExpr(state, expr);
    } else if (branch.subtrees.size() == 2) {
        auto [_, pre2] = vectorView<2>(branch.subtrees);
        return genPre2(state, pre2);
    } else {
        assert((false));
    }
}

std::optional<Type> genBinOp(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    if (branch.nt == NonTerminals::PRE2) return genPre2(state, t);  // edge case
    assert((!state.error && (branch.nt == NonTerminals::PRE12 
        || branch.nt == NonTerminals::PRE11 
        || branch.nt == NonTerminals::PRE7 
        || branch.nt == NonTerminals::PRE6 
        || branch.nt == NonTerminals::PRE4
        || branch.nt == NonTerminals::PRE3)));
    if (branch.subtrees.size() == 1)
        return genBinOp(state, branch.subtrees.at(0));
    auto [left, eq, right] = vectorView<3>(branch.subtrees);
    auto leftType = genBinOp(state, left);
    if (leftType == Type::NUMBER_TYPE) {
        auto rightType = genBinOp(state, right);
        if (rightType == Type::NUMBER_TYPE) {
            return Type::NUMBER_TYPE;
        } else if (rightType.has_value()) {
            state.error = invalidTypeError(std::get<Token>(eq), Type::NUMBER_TYPE, *rightType);
        }
    } else if (leftType.has_value()) {
        state.error = invalidTypeError(std::get<Token>(eq), Type::NUMBER_TYPE, *leftType);
    }
    return std::nullopt;
}

std::optional<Type> genPre14(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::PRE14));
    if (branch.subtrees.size() == 1) 
        return genBinOp(state, branch.subtrees.at(0));
    auto [id, eq, pre14] = vectorView<3>(branch.subtrees);
    auto &idAsToken = std::get<Token>(id);
    auto idAsStr = idAsToken.toString();
    if (auto vid = state.context.searchAllScopes(idAsStr)) {
        auto idType = state.symbolTable.vidToType[*vid];
        auto exprAsType = genPre14(state, pre14);
        if (exprAsType == idType) {
            state.symbolTable.tokenToVID.emplace(&idAsToken, *vid);
            return exprAsType;
        } else {
            // types do not match
            if (exprAsType.has_value()) {
                state.error = assignMismatchError(std::get<Token>(eq), *exprAsType, idType);
            }
            return std::nullopt;
        }
    } else {
        // id not declared
        state.error = undefinedVariableError(idAsToken);
        return std::nullopt;
    }
    
}

std::optional<Type> genExpr(SemanticState &state, const Tree &t) {

    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::EXPR));
    return genPre14(state, branch.subtrees.at(0));
}

std::optional<Type> genVarDef(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::VARDEF));
    if (branch.subtrees.size() == 2) {
        auto [type, id] = vectorView<2>(branch.subtrees);
        auto idAsToken = std::get<Token>(id);
        auto idAsStr = idAsToken.toString();
        auto typeAsType = tokenToType(std::get<Token>(type));
        if (!state.context.isDefinedInCurrentScope(idAsStr)) {
            // id does not exists
            auto vid = state.context.defineVariableInScope(idAsStr);
            state.symbolTable.vidToType.emplace(vid, typeAsType);   // add vid entry
            return Type::VOID_TYPE;
        } else {
            // id already exist (error)
            state.error = redefinedVariableError(idAsToken);
            return std::nullopt;
        }
    } else if (branch.subtrees.size() == 4) {
        auto [type, id, _, expr] = vectorView<4>(branch.subtrees);
        auto &idAsToken = std::get<Token>(id);
        auto idAsStr = idAsToken.toString();
        auto idTypeAsType = tokenToType(std::get<Token>(type));
        if (!state.context.isDefinedInCurrentScope(idAsStr)) {
            auto exprAsType = genExpr(state, expr);
            if (exprAsType == idTypeAsType) {
                auto vid = state.context.defineVariableInScope(idAsStr);
                state.symbolTable.vidToType.emplace(vid, idTypeAsType);   // add vid entry
                state.symbolTable.tokenToVID.emplace(&idAsToken, vid);    // map id token to vid
                return Type::VOID_TYPE;
            } else {
                if (exprAsType.has_value())
                    state.error = assignMismatchError(idAsToken, *exprAsType, idTypeAsType);
                return std::nullopt;
            }
        } else {
            state.error = redefinedVariableError(idAsToken);
            return std::nullopt;
        }
    } else {
        assert((false));
    }
}

std::optional<Type> genBStmts(SemanticState &state, const Tree &t);
std::optional<Type> genBlock(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::BLOCK));
    state.context.enterScope();
    auto ret = genBStmts(state, branch.subtrees.at(1));
    state.context.exitScope();
    return ret;
}

std::optional<Type> genBStmt(SemanticState &state, const Tree &t);
std::optional<Type> genIfCont(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::IFCONT));
    if (branch.subtrees.size() == 0) return Type::VOID_TYPE;
    else {
        auto [_, bstmt] = vectorView<2>(branch.subtrees);
        return genBStmt(state, bstmt);
    }
}

std::optional<Type> genBStmt(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::BSTMT));
    if (std::holds_alternative<Token>(branch.subtrees.at(0))) {
        
        switch(std::get<Token>(branch.subtrees.at(0)).kind) {
            case IF: {

                auto [_1, _2, expr, _3, bstmt, ifcont] = vectorView<6>(branch.subtrees);

                auto res = genExpr(state, expr);
                if (res == Type::NUMBER_TYPE) {

                    if (auto res2 = genBStmt(state, bstmt)) {
                        return genIfCont(state, ifcont);
                    } else {
                        return std::nullopt;
                    }
                } else {
                    if (res.has_value())
                        state.error = invalidTypeError(std::get<Token>(_1), Type::NUMBER_TYPE, *res);
                    return std::nullopt;
                }
                
            }
            case WHILE: {
                auto [_1, _2, expr, _3, bstmt] = vectorView<5>(branch.subtrees);
                auto res = genExpr(state, expr);
                if (res == Type::NUMBER_TYPE) {
                    return genBStmt(state, bstmt);
                    if (res.has_value())
                        invalidTypeError(std::get<Token>(_1), Type::NUMBER_TYPE, *res);
                    return std::nullopt;
                }
            }
            default: assert((false));
        }
    } else {

        // first is non terminal
        if (branch.subtrees.size() == 2) {
            auto [vardefOrExpr, _] = vectorView<2>(branch.subtrees);
            switch (std::get<Branch>(vardefOrExpr).nt) {
                case NonTerminals::VARDEF:
                    return genVarDef(state, vardefOrExpr) == Type::VOID_TYPE
                        ? std::make_optional(Type::VOID_TYPE)
                        : std::nullopt;
                case NonTerminals::EXPR:
                    return genExpr(state, vardefOrExpr).has_value()
                        ? std::make_optional(Type::VOID_TYPE)
                        : std::nullopt;
                default:
                    assert((false));
            }
        } else if (branch.subtrees.size() == 1) {

            return genBlock(state, branch.subtrees.at(0));
        } else {
            assert((false));
        }
    }
    
}

std::optional<Type> genBStmts(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::BSTMTS));
    if (branch.subtrees.size() == 0) {
        return Type::VOID_TYPE;
    }
    else if (branch.subtrees.size() == 2) {
        auto [bstmts, bstmt] = vectorView<2>(branch.subtrees);
        if (genBStmts(state, bstmts) == Type::VOID_TYPE && genBStmt(state, bstmt) == Type::VOID_TYPE) {
            return Type::VOID_TYPE;
        } else {
            return std::nullopt;
        }
    } else {
        assert((false));
    }
}

std::optional<Type> genStart(SemanticState &state, const Tree &t) {
    auto &branch = std::get<Branch>(t);
    assert((!state.error && branch.nt == NonTerminals::START));
    
    auto ret = genBStmts(state, branch.subtrees.at(0));
    state.context.exitScope();
    return ret;
}

void initState(SemanticState &state) {
    state.context.enterScope();
    state.symbolTable.vidToType = {
        {state.context.defineVariableInScope("TARGET"), Type::ENTITY_TYPE},
        {state.context.defineVariableInScope("SELF"), Type::ENTITY_TYPE}
    };
    state.symbolTable.funNameToType = {
        {"round", {{Type::NUMBER_TYPE}, Type::NUMBER_TYPE}},
        {"sqrt", {{Type::NUMBER_TYPE}, Type::NUMBER_TYPE}},
        {"sin", {{Type::NUMBER_TYPE}, Type::NUMBER_TYPE}},
        {"cos", {{Type::NUMBER_TYPE}, Type::NUMBER_TYPE}},
        {"makevec", {{Type::NUMBER_TYPE, Type::NUMBER_TYPE, Type::NUMBER_TYPE}, Type::VECTOR_TYPE}},
        {"vx", {{Type::VECTOR_TYPE}, Type::NUMBER_TYPE}},
        {"vy", {{Type::VECTOR_TYPE}, Type::NUMBER_TYPE}},
        {"vz", {{Type::VECTOR_TYPE}, Type::NUMBER_TYPE}},
        {"vadd", {{Type::VECTOR_TYPE, Type::VECTOR_TYPE}, Type::VECTOR_TYPE}},
        {"vsub", {{Type::VECTOR_TYPE, Type::VECTOR_TYPE}, Type::VECTOR_TYPE}},
        {"vmul", {{Type::VECTOR_TYPE, Type::NUMBER_TYPE}, Type::VECTOR_TYPE}},
        {"vdiv", {{Type::VECTOR_TYPE, Type::NUMBER_TYPE}, Type::VECTOR_TYPE}},
        {"vdist", {{Type::VECTOR_TYPE}, Type::NUMBER_TYPE}},
        {"vnorm", {{Type::VECTOR_TYPE}, Type::VECTOR_TYPE}},
        {"vdot", {{Type::VECTOR_TYPE, Type::VECTOR_TYPE}, Type::NUMBER_TYPE}},
        {"vcross", {{Type::VECTOR_TYPE, Type::VECTOR_TYPE}, Type::VECTOR_TYPE}},
        {"slength", {{Type::STRING_TYPE}, Type::NUMBER_TYPE}},
        {"scharat", {{Type::STRING_TYPE, Type::NUMBER_TYPE}, Type::STRING_TYPE}},
        {"scodeat", {{Type::STRING_TYPE, Type::NUMBER_TYPE}, Type::NUMBER_TYPE}},
        {"ssubstr", {{Type::STRING_TYPE, Type::NUMBER_TYPE, Type::NUMBER_TYPE}, Type::STRING_TYPE}},
        {"sconcat", {{Type::STRING_TYPE, Type::STRING_TYPE}, Type::STRING_TYPE}},
        {"ssearch", {{Type::STRING_TYPE, Type::STRING_TYPE}, Type::NUMBER_TYPE}},
        {"scmp", {{Type::STRING_TYPE, Type::STRING_TYPE}, Type::NUMBER_TYPE}},
        {"sify", {{Type::OBJECT_TYPE}, Type::STRING_TYPE}},
        {"sifyd", {{Type::OBJECT_TYPE}, Type::STRING_TYPE}},
        {"print", {{Type::OBJECT_TYPE}, Type::VOID_TYPE}},
        {"findent", {{Type::VECTOR_TYPE, Type::NUMBER_TYPE}, Type::ENTITY_TYPE}},
        {"entpos", {{Type::ENTITY_TYPE}, Type::VECTOR_TYPE}},
        {"entvel", {{Type::ENTITY_TYPE}, Type::VECTOR_TYPE}},
        {"entfacing", {{Type::ENTITY_TYPE}, Type::VECTOR_TYPE}},
        {"checkblock", {{Type::VECTOR_TYPE, Type::STRING_TYPE}, Type::NUMBER_TYPE}},
        {"accelent", {{Type::ENTITY_TYPE, Type::VECTOR_TYPE}, Type::VOID_TYPE}},
        {"damageent", {{Type::ENTITY_TYPE, Type::NUMBER_TYPE}, Type::VOID_TYPE}},
        {"mountent", {{Type::ENTITY_TYPE, Type::ENTITY_TYPE}, Type::VOID_TYPE}},
        {"fireballpwr", {{Type::ENTITY_TYPE, Type::NUMBER_TYPE}, Type::VOID_TYPE}},
        {"explode", {{Type::VECTOR_TYPE, Type::NUMBER_TYPE}, Type::VOID_TYPE}},
        {"placeblock", {{Type::VECTOR_TYPE, Type::STRING_TYPE}, Type::VOID_TYPE}},
        {"destroyblock", {{Type::VECTOR_TYPE}, Type::VOID_TYPE}},
        {"lightning", {{Type::VECTOR_TYPE}, Type::VOID_TYPE}},
        {"summon", {{Type::VECTOR_TYPE, Type::STRING_TYPE}, Type::ENTITY_TYPE}},
        {"wait", {{Type::NUMBER_TYPE}, Type::VOID_TYPE}}
    };
}

std::variant<CompilerError, SymbolTable> generateSymbolTable(const Tree &t) {
    SymbolTable symbolTable;
    
    SemanticState state(symbolTable);
    initState(state);
    assert(!state.error);
    genStart(state, t);
    if (state.error) {
        return *state.error;
    } else {
        return symbolTable;
    }
}

