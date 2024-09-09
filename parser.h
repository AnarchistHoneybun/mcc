//
// Created by vrin on 9/9/24.
//

#ifndef MCC_PARSER_H
#define MCC_PARSER_H

#endif //MCC_PARSER_H

#pragma once
#include <vector>
#include <stdexcept>
#include "ast.h"
#include "lexer.h"

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
};

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message) : std::runtime_error(message) {}
};