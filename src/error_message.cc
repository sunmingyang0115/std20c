#include "error_message.hh"
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

const char* BOLD_RED = "\033[1;31m";
const char* DEFAULT = "\e[0m";

std::tuple<size_t, size_t> getLnCol(const std::string &contents, std::string::const_iterator it) {
    size_t line = 1;
    size_t col = 1;
    for (auto iter = contents.begin(); iter != it; ++iter) {
        if (*iter == '\n') {
            ++line;
            col = 1;
        } else {
            ++col;
        }
    }
    return std::make_tuple(line, col);
}
std::pair<std::string::const_iterator, std::string::const_iterator> getSurroundings(const std::string &contents, std::string::const_iterator it, size_t length) {
    const size_t maxChars = 10;
    auto start = it;
    auto end = it + length;
    // Move start iterator back up to max_chars or until a newline is found
    for (size_t i = 0; i < maxChars && start != contents.begin(); start--) {
        if (*start == '\n') {
            ++start;
            break;
        }
    }
    // Move end iterator forward up to max_chars or until a newline is found
    for (size_t i = 0; i < maxChars && end != contents.end(); end++) {
        if (*end == '\n') {
            break;
        }
    }
    return std::make_pair(start, end);
}

int generateMessage(const std::string &contents, CompilerError error) {
    auto errorStart = contents.begin() + error.start;
    auto errorType = [&]() {
        switch (error.type) {
        case CompilerError::Scan:
            return "scan error";
        case CompilerError::Parse:
            return "parse error";
        case CompilerError::Type:
            return "type error";
        case CompilerError::Compile:
            return "compile error";
        default:
            return "unknown error";
        };
    }();

    // find ln and col of contents
    auto [ln, col] = getLnCol(contents, errorStart);
    std::cerr << ln << ":" << col << ": " << BOLD_RED << errorType << ": " << DEFAULT << error.errorMessage << "\n";

    auto [beginIt, endIt] = getSurroundings(contents, errorStart, error.length);

    // clipped text on the sides of the error position
    std::string beginContext(beginIt, errorStart);
    std::string errorContext(errorStart, errorStart + error.length);
    std::string endContext(std::string(errorStart + error.length, endIt));

    std::string strLn = std::to_string(ln);
    size_t headerSize = std::max(strLn.size(), static_cast<size_t>(5));
    std::cerr << std::setw(headerSize) << strLn;
    std::cerr << " | " << beginContext << BOLD_RED << errorContext << DEFAULT << endContext << "\n";
    std::cerr << std::setw(headerSize) << "";
    std::cerr << " | " << std::setw(beginContext.size()) << "" << BOLD_RED << std::setw(errorContext.size()) << std::setfill('^') << "" << DEFAULT << "\n";
    return 1;
}
