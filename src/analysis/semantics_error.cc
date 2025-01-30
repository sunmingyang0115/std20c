#include "semantics.hh"
#include <numeric>

std::string typeToErrorString(Type t) {
    switch (t) {
    case Type::STRING_TYPE:
        return "String";
    case Type::NUMBER_TYPE:
        return "Number";
    case Type::ENTITY_TYPE:
        return "Entity";
    case Type::VECTOR_TYPE:
        return "Vector";
    case Type::VOID_TYPE:
        return "Void";
    default:
        throw std::runtime_error("Unknown Type");
    }
}
std::string typesToErrorString(const std::vector<Type> &types) {
    if (types.empty()) return "";
    return std::accumulate(std::next(types.begin()), types.end(), typeToErrorString(types[0]),
                           [](std::string a, Type b) { return std::move(a) + ", " + typeToErrorString(b); });
}

CompilerError invalidArgumentError(const Token &id, const std::vector<Type> &expected, const std::vector<Type> &obtained) {
    return CompilerError(
        CompilerError::Type::TYPE, 
        id.begin, 
        id.length, 
        "invalid arguments for function (expected (" + typesToErrorString(expected) + ") but got (" + typesToErrorString(obtained) + "))");
}
CompilerError undefinedVariableError(const Token &id) {
    return CompilerError(
        CompilerError::Type::TYPE, 
        id.begin, 
        id.length, 
        "use of undefined variable identifier");
}
CompilerError undefinedFunctionError(const Token &id) {
    return CompilerError(
        CompilerError::Type::TYPE, 
        id.begin, 
        id.length, 
        "use of undefined function identifier");
}
CompilerError redefinedVariableError(const Token &id) {
    return CompilerError(
        CompilerError::Type::TYPE, 
        id.begin, 
        id.length, 
        "redefinition of existing variable identifier");
}

CompilerError assignMismatchError(const Token &location, const Type &from, const Type &into) {
    return CompilerError(
        CompilerError::Type::TYPE, 
        location.begin, 
        location.length, 
        "assign type mismatch (cannot assign " + typeToErrorString(from) + " into " + typeToErrorString(into) + ")");
}

CompilerError invalidTypeError(const Token &location, const Type &expected, const Type &obtained) {
    return CompilerError(
        CompilerError::Type::TYPE, 
        location.begin, 
        location.length, 
        "invalid type for operation (expected " + typeToErrorString(expected) + " but got " + typeToErrorString(obtained) + ")");
}
