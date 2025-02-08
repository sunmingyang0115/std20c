#ifndef ERROR_MESSAGE_HH
#define ERROR_MESSAGE_HH
#include <string>
#include <cstddef>

struct CompilerError {
    enum Type { SCAN, PARSE, TYPE, COMPILE } type;
    std::string::const_iterator errorPosition;
    size_t errorLength;
    std::string errorMessage;
    CompilerError(enum Type type, std::string::const_iterator errorPosition, size_t errorLength, std::string errorMessage): 
        type(type), errorPosition(errorPosition), errorLength(errorLength), errorMessage(errorMessage) {}
};

// input => original; position
int generateErrorMessage(const std::string &contents, CompilerError error);

#endif