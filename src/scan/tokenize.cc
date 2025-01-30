#include "tokenize.hh"
#include <optional>

std::optional<Token> scanSingleToken(std::string::const_iterator &begin, const std::string::const_iterator &end) {
    // longest match > order of appearence
    std::size_t longest_match = 0;
    std::optional<Token> result = std::optional<Token>();
    for (auto &p : tokenizationRules) {
        auto i = std::sregex_iterator(begin, end, p.regex);
        if (i != std::sregex_iterator() && i->str().size() > longest_match) {
            longest_match = i->str().size();
            result = std::optional<Token>(Token(begin, i->str().length(), p.kind));
        }
    }
    if (result.has_value()) {
        begin += result->length;
    }
    return result;
}

std::variant<CompilerError, std::vector<Token>> maximalMunch(const std::string &s) {
    std::vector<Token> v;
    std::string::const_iterator begin = s.begin();
    std::string::const_iterator end = s.end();
    while(begin != end) {
        std::optional<Token> result = scanSingleToken(begin, end);
        if (result.has_value()) {
            v.push_back(result.value());
        } else {
            return CompilerError(CompilerError::Type::SCAN, begin, 1, "Unrecognized Token");
        }
    }
    return v;
}


