#ifndef LOWER_HH
#define LOWER_HH
#include <std20c/compilation.hh>
#include <std20c/language.hh>
#include <std20c/ir.hh>

IR generateIR(const SymbolTable &, const Tree &);

#endif