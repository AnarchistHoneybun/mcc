#include "lexer.h"
#include <stdexcept>

Lexer::Lexer(const std::string &input) : m_input(input), m_position(0) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (m_position < m_input.length()) {
        skipWhitespaceAndComments();
        if (m_position < m_input.length()) {
            tokens.push_back(getNextToken());
        }
    }
    return tokens;
}

void Lexer::skipWhitespaceAndComments() {
    while (m_position < m_input.length()) {
        if (std::isspace(m_input[m_position])) {
            ++m_position;
        } else if (m_input[m_position] == '/' && m_position + 1 < m_input.length()) {
            if (m_input[m_position + 1] == '/') {
                skipSingleLineComment();
            } else if (m_input[m_position + 1] == '*') {
                skipMultiLineComment();
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

void Lexer::skipSingleLineComment() {
    m_position += 2;  // Skip '//'
    while (m_position < m_input.length() && m_input[m_position] != '\n') {
        ++m_position;
    }
}

void Lexer::skipMultiLineComment() {
    m_position += 2;  // Skip '/*'
    while (m_position < m_input.length()) {
        if (m_input[m_position] == '*' && m_position + 1 < m_input.length() && m_input[m_position + 1] == '/') {
            m_position += 2;  // Skip '*/'
            return;
        }
        ++m_position;
    }
    throw std::runtime_error("Unterminated multi-line comment");
}

Token Lexer::getNextToken() {
    auto [type, value] = findLongestMatch();
    if (type == TokenType::IDENTIFIER && (value == "int" || value == "void" || value == "return")) {
        if (value == "int") type = TokenType::INT_KEYWORD;
        else if (value == "void") type = TokenType::VOID_KEYWORD;
        else type = TokenType::RETURN_KEYWORD;
    }
    m_position += value.length();
    return {type, value};
}

std::pair<TokenType, std::string> Lexer::findLongestMatch() {
    static const std::vector<std::pair<TokenType, std::regex>> token_regexes = {
            {TokenType::IDENTIFIER,  std::regex(R"([a-zA-Z_]\w*\b)")},
            {TokenType::CONSTANT,    std::regex(R"([0-9]+\b)")},
            {TokenType::OPEN_PAREN,  std::regex(R"(\()")},
            {TokenType::CLOSE_PAREN, std::regex(R"(\))")},
            {TokenType::OPEN_BRACE,  std::regex(R"(\{)")},
            {TokenType::CLOSE_BRACE, std::regex(R"(\})")},
            {TokenType::SEMICOLON,   std::regex(R"(;)")}
    };

    std::pair<TokenType, std::string> longest_match = {TokenType::IDENTIFIER, ""};
    std::string remaining = m_input.substr(m_position);
    for (const auto &[type, regex]: token_regexes) {
        std::smatch match;
        if (std::regex_search(remaining.cbegin(), remaining.cend(), match, regex,
                              std::regex_constants::match_continuous)) {
            if (match.length() > longest_match.second.length()) {
                longest_match = {type, match.str()};
            }
        }
    }

    if (longest_match.second.empty()) {
        throw std::runtime_error("Invalid token at position " + std::to_string(m_position));
    }

    return longest_match;
}