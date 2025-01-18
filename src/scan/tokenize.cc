#include "tokenize.hh"
#include <optional>

std::optional<Token> scanSingleToken(std::string::iterator &begin, const std::string::iterator &end) {
    // longest match > order of appearence
    std::size_t longest_match = 0;
    std::optional<Token> result = std::optional<Token>();
    for (auto &p : tokenizationRules) {
        auto i = std::sregex_iterator(begin, end, p.regex);
        if (i != std::sregex_iterator() && i->str().size() > longest_match) {
            longest_match = i->str().size();
            result = std::optional<Token>(Token(i->str(), p.kind));
        }
    }
    if (result.has_value()) {
        begin += result.value().lexeme.size();
    }
    return result;
}

std::variant<CompilerError, std::vector<Token>> maximalMunch(std::string s) {
    std::vector<Token> v;
    std::string::iterator begin = s.begin();
    std::string::iterator end = s.end();
    while(begin != end) {
        std::optional<Token> result = scanSingleToken(begin, end);
        if (result.has_value()) {
            v.push_back(result.value());
        } else {
            return CompilerError{CompilerError::Type::Scan, static_cast<size_t>(std::distance(s.begin(), begin)), 1, "Unrecognized Token"};
        }
    }
    return v;
}


