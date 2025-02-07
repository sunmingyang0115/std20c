#ifndef SCOPE_HH
#define SCOPE_HH
#include "../compilation.hh"
#include <deque>
#include <optional>
#include <vector>

// data structure used to create a symbol table by mapping variable names in scopes to VariableID
struct VariableScopeContext {
    using Scope = std::map<std::string, VariableID>;
    std::deque<Scope> scopes{};
    VariableID idCounter{0};

    bool isDefinedInCurrentScope(std::string varName);
    VariableID defineVariableInScope(std::string varName);
    std::optional<VariableID> searchAllScopes(std::string varName);
    void enterScope() { scopes.emplace_back(); }
    void exitScope() { scopes.pop_back(); }
};

#endif