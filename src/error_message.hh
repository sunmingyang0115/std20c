#ifndef ERROR_MESSAGE_HH
#define ERROR_MESSAGE_HH
#include <string>
#include <cstddef>

struct CompilerError {
    enum Type {
        Scan,
        Parse,
        Type,
        Compile
    } type;
    size_t start;
    size_t length;
    std::string errorMessage;
    CompilerError(enum Type type, size_t start, size_t length, std::string errorMessage): type(type), start(start), length(length), errorMessage(errorMessage) {};
};

// input => original; position
int generateMessage(const std::string &contents, CompilerError error);

#endif