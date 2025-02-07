#ifndef LOWER_HH
#define LOWER_HH
#include "../compilation.hh"
#include "../language.hh"
#include "../ir.hh"

IR generateIR(const SymbolTable &, const Tree &);

#endif