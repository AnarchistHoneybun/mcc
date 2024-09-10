//
// Created by vrin on 9/9/24.
//

#ifndef MCC_AST_H
#define MCC_AST_H

#endif //MCC_AST_H
#pragma once
#include <string>
#include <memory>
#include <sstream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string prettyPrint(int indent = 0) const = 0;

protected:
    static std::string indentString(int indent) {
        return std::string(indent * 2, ' ');
    }
};

class Exp : public ASTNode {
public:
    virtual ~Exp() = default;
};

class Constant : public Exp {
public:
    explicit Constant(int value) : value(value) {}
    int value;

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream oss;
        oss << indentString(indent) << "Constant(" << value << ")";
        return oss.str();
    }
};

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class Return : public Statement {
public:
    explicit Return(std::unique_ptr<Exp> exp) : exp(std::move(exp)) {}
    std::unique_ptr<Exp> exp;

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream oss;
        oss << indentString(indent) << "Return(\n"
            << exp->prettyPrint(indent + 1) << "\n"
            << indentString(indent) << ")";
        return oss.str();
    }
};

class Function : public ASTNode {
public:
    Function(std::string name, std::unique_ptr<Statement> body)
            : name(std::move(name)), body(std::move(body)) {}
    std::string name;
    std::unique_ptr<Statement> body;

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream oss;
        oss << indentString(indent) << "Function(\n"
            << indentString(indent + 1) << "name=\"" << name << "\",\n"
            << indentString(indent + 1) << "body=\n"
            << body->prettyPrint(indent + 2) << "\n"
            << indentString(indent) << ")";
        return oss.str();
    }
};

class Program : public ASTNode {
public:
    explicit Program(std::unique_ptr<Function> function)
            : function(std::move(function)) {}
    std::unique_ptr<Function> function;

    std::string prettyPrint(int indent = 0) const override {
        std::ostringstream oss;
        oss << indentString(indent) << "Program(\n"
            << function->prettyPrint(indent + 1) << "\n"
            << indentString(indent) << ")";
        return oss.str();
    }
};