#ifndef SEMANTICS_HH
#define SEMANTICS_HH
#include <std20c/compilation.hh>
#include <std20c/error_message.hh>
#include <std20c/language.hh>
#include <vector>

std::variant<CompilerError, SymbolTable> generateSymbolTable(const Tree &t);

// function call wrong number/types of arguments
CompilerError invalidArgumentError(const Token &id, const std::vector<Type> &expected, const std::vector<Type> &obtained);

// using variable/function without defining it
CompilerError undefinedVariableError(const Token &id);
CompilerError undefinedFunctionError(const Token &id);

// defining a variable a second time
CompilerError redefinedVariableError(const Token &id);

// assigning to variable of wrong type
CompilerError assignMismatchError(const Token &location, const Type &from, const Type &into);

// operations on non-number types
CompilerError invalidTypeError(const Token &location, const Type &expected, const Type &obtained);

#endif