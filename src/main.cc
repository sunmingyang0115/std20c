#include <std20c/compilation.hh>
#include <std20c/error_message.hh>
#include <std20c/language.hh>
#include "scan/tokenize.hh"
#include "parse/parser.hh"
#include "analysis/semantics.hh"
#include "codegen/lower.hh"
#include "debug.hh"
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

std::variant<IR, CompilerError> compile(const std::string &code) {
    auto tryScan = maximalMunch(code);
    if (std::holds_alternative<CompilerError>(tryScan)) {
        return std::get<CompilerError>(tryScan);
    }
    auto tryParse = earleyParser(std::get<std::vector<Token>>(tryScan));
    if (std::holds_alternative<CompilerError>(tryParse)) {
        return std::get<CompilerError>(tryParse);
    }
    auto &parseTree = std::get<Tree>(tryParse);
    auto tryAnalyze = generateSymbolTable(parseTree);
    if (std::holds_alternative<CompilerError>(tryAnalyze)) {
        return std::get<CompilerError>(tryAnalyze);
    }
    auto tryCodeGen = generateIR(std::get<SymbolTable>(tryAnalyze), parseTree);
    // in current implementation, generateIR is no fail
    return tryCodeGen;
} 

int main(int argc, char* argv[]) {
    std::string executable = argv[0];
    std::string outfile = "a.out";
    std::optional<std::string> infile;
    for (int i = 1; i < argc; i++) {
        if (!std::strcmp(argv[i], "-o")) {
            if (i+1 < argc) {
                outfile = argv[++i];
            } else {
                std::cerr << executable << ": missing filename after `" << argv[i] << "`\n";
                return 1;
            }
        } else if (!infile.has_value()) {
            infile = argv[i];
        } else {
            std::cerr << executable << ": unrecognized command-line option `" << argv[i] << "`\n";
            return 1;
        }
    }
    if (!infile.has_value()) {
        std::cerr << executable << ": no input files\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << executable << ": cannot find " << argv[1] << ": No such file or directory\n";
        return 1;
    }
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto tryCompile = compile(contents);
    if (std::holds_alternative<CompilerError>(tryCompile)) {
        return generateErrorMessage(contents, std::get<CompilerError>(tryCompile));
    }
    std::ofstream out;
    out.open(outfile);
    out << std::get<IR>(tryCompile) << std::endl;
}