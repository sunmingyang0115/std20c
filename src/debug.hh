#ifndef DEBUG_HH
#define DEBUG_HH
#include <ostream>
#include <std20c/ir.hh>
#include <std20c/compilation.hh>
#include <std20c/language.hh>

std::ostream& operator<<(std::ostream &, Terminals) ;
std::ostream& operator<<(std::ostream &, NonTerminals);
std::ostream& operator<<(std::ostream &, Type);
std::ostream& operator<<(std::ostream &, const Tree &);
std::ostream& operator<<(std::ostream &, const IR &);
std::ostream& operator<<(std::ostream &, const SymbolTable &);

#endif