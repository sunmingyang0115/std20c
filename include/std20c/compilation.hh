#ifndef COMPILER_HH
#define COMPILER_HH

#include <map>
#include <string>
#include <vector>
enum Type {
    STRING_TYPE, NUMBER_TYPE, ENTITY_TYPE, VECTOR_TYPE, VOID_TYPE, OBJECT_TYPE
};
struct Function {
    std::vector<Type> args;
    Type ret;
};

struct Token;

/**
    SymbolTable has two main functions: eliminates scopes for variables and resolves variable/function types 
    This will be generated in the semantic analysis step, which is needed for lowering
 */
using VariableID = std::size_t;
struct SymbolTable {
    /**
        tokenToVID => maps all ID (variable) Tokens to a unique variable identifier (VID)
            excludes ID tokens referenced in declaration
            needed as strings by themselves are not enough to due to scopes
     */
    std::map<const Token*, VariableID> tokenToVID;
    //  vidToType => maps VID to respective type
    std::map<VariableID, Type> vidToType;
    //  maps function name to respective function type
    std::map<std::string, Function> funNameToType; 
};


#endif