#pragma once
#include <string>
#include <vector>
#include <regex>

enum class TokenType {
    IDENTIFIER,
    CONSTANT,
    INT_KEYWORD,
    VOID_KEYWORD,
    RETURN_KEYWORD,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string m_input;
    size_t m_position;

    void skipWhitespaceAndComments();
    void skipSingleLineComment();
    void skipMultiLineComment();
    Token getNextToken();
    std::pair<TokenType, std::string> findLongestMatch();
};