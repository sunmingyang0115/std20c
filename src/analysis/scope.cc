#include "scope.hh"
#include <deque>
#include <iostream>
#include <vector>

std::optional<SymbolTable::VariableID> VariableScopeContext::searchAllScopes(std::string varName) {
    // std::cerr << "123\n";
    for (auto it = this->scopes.rbegin(); it != this->scopes.rend(); ++it) {
        auto varID = it->find(varName);
        if (varID != it->end()) {
            return varID->second;
        }
    }
    return std::nullopt;
}

SymbolTable::VariableID VariableScopeContext::defineVariableInScope(std::string varName) {
    this->scopes.back().emplace(varName, this->idCounter);
    return this->idCounter++;
}

bool VariableScopeContext::isDefinedInCurrentScope(std::string varName) {
    return this->scopes.back().find(varName) != this->scopes.back().end();
}
