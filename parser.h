#pragma once
#include <vector>
#include <stdexcept>
#include <string>
#include "ast.h"
#include "lexer.h"

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<Program> parse();

private:
    std::vector<Token> m_tokens;
    size_t m_position;

    std::unique_ptr<Function> parseFunction();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Exp> parseExp();
    void expect(TokenType type);
    Token consumeToken();
    bool match(TokenType type);
    std::string tokenTypeToString(TokenType type) const;
};