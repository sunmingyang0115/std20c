#ifndef SCOPE_HH
#define SCOPE_HH
#include "../compilation.hh"
#include <deque>
#include <optional>
#include <vector>

// data structure used to create a symbol table by mapping variable names in scopes to VariableID
struct VariableScopeContext {
    using Scope = std::map<std::string, SymbolTable::VariableID>;
    std::deque<Scope> scopes{};
    SymbolTable::VariableID idCounter{0};

    bool isDefinedInCurrentScope(std::string varName);
    SymbolTable::VariableID defineVariableInScope(std::string varName);
    std::optional<SymbolTable::VariableID> searchAllScopes(std::string varName);
    void enterScope() { scopes.emplace_back(); }
    void exitScope() { scopes.pop_back(); }
};

#endif