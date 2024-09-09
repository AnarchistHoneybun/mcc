//
// Created by vrin on 9/9/24.
//

#include "parser.h"

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_position(0) {}

std::unique_ptr<Program> Parser::parse() {
    auto function = parseFunction();
    if (m_position < m_tokens.size()) {
        throw ParseError("Unexpected tokens after function definition");
    }
    return std::make_unique<Program>(std::move(function));
}

std::unique_ptr<Function> Parser::parseFunction() {
    expect(TokenType::INT_KEYWORD);
    auto name = consumeToken();
    if (name.type != TokenType::IDENTIFIER) {
        throw ParseError("Expected function name");
    }
    expect(TokenType::OPEN_PAREN);
    expect(TokenType::VOID_KEYWORD);
    expect(TokenType::CLOSE_PAREN);
    expect(TokenType::OPEN_BRACE);
    auto body = parseStatement();
    expect(TokenType::CLOSE_BRACE);
    return std::make_unique<Function>(name.value, std::move(body));
}

std::unique_ptr<Statement> Parser::parseStatement() {
    expect(TokenType::RETURN_KEYWORD);
    auto exp = parseExp();
    expect(TokenType::SEMICOLON);
    return std::make_unique<Return>(std::move(exp));
}

std::unique_ptr<Exp> Parser::parseExp() {
    auto token = consumeToken();
    if (token.type != TokenType::CONSTANT) {
        throw ParseError("Expected constant expression");
    }
    return std::make_unique<Constant>(std::stoi(token.value));
}

void Parser::expect(TokenType type) {
    if (!match(type)) {
        throw ParseError("Unexpected token");
    }
}

Token Parser::consumeToken() {
    if (m_position >= m_tokens.size()) {
        throw ParseError("Unexpected end of input");
    }
    return m_tokens[m_position++];
}

bool Parser::match(TokenType type) {
    if (m_position < m_tokens.size() && m_tokens[m_position].type == type) {
        m_position++;
        return true;
    }
    return false;
}