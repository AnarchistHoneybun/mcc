//
// Created by vrin on 9/9/24.
//

#ifndef MCC_AST_H
#define MCC_AST_H

#endif //MCC_AST_H

#pragma once
#include <string>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Exp : public ASTNode {
public:
    virtual ~Exp() = default;
};

class Constant : public Exp {
public:
    explicit Constant(int value) : value(value) {}
    int value;
};

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class Return : public Statement {
public:
    explicit Return(std::unique_ptr<Exp> exp) : exp(std::move(exp)) {}
    std::unique_ptr<Exp> exp;
};

class Function : public ASTNode {
public:
    Function(std::string name, std::unique_ptr<Statement> body)
            : name(std::move(name)), body(std::move(body)) {}
    std::string name;
    std::unique_ptr<Statement> body;
};

class Program : public ASTNode {
public:
    explicit Program(std::unique_ptr<Function> function)
            : function(std::move(function)) {}
    std::unique_ptr<Function> function;
};