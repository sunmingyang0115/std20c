#include "scope.hh"

std::optional<VariableID> VariableScopeContext::searchAllScopes(std::string varName) {
    for (auto it = this->scopes.rbegin(); it != this->scopes.rend(); ++it) {
        auto varID = it->find(varName);
        if (varID != it->end()) {
            return varID->second;
        }
    }
    return std::nullopt;
}

VariableID VariableScopeContext::defineVariableInScope(std::string varName) {
    this->scopes.back().emplace(varName, this->idCounter);
    return this->idCounter++;
}

bool VariableScopeContext::isDefinedInCurrentScope(std::string varName) {
    return this->scopes.back().find(varName) != this->scopes.back().end();
}
