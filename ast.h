#pragma once

#include <string>
#include <memory>
#include <sstream>
#include "assembly_ast.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual std::string prettyPrint(int indent = 0) const = 0;

    virtual std::unique_ptr<assembly::AsmNode> codegen() const = 0;

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

    std::unique_ptr<assembly::AsmNode> codegen() const override {
        return std::make_unique<assembly::Imm>(value);
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

    std::unique_ptr<assembly::AsmNode> codegen() const override {
        auto expCode = exp->codegen();
        auto *operand = dynamic_cast<assembly::Operand *>(expCode.get());
        if (!operand) {
            throw std::runtime_error("Expression did not generate an Operand");
        }
        auto movInstr = std::make_unique<assembly::Mov>(
                std::unique_ptr<assembly::Operand>(operand),
                std::make_unique<assembly::Register>("eax")
        );
        expCode.release(); // Release ownership as it's now managed by movInstr
        return movInstr;
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

    std::unique_ptr<assembly::AsmNode> codegen() const override {
        std::vector<std::unique_ptr<assembly::Instruction>> instructions;
        instructions.push_back(std::unique_ptr<assembly::Instruction>(
                dynamic_cast<assembly::Instruction *>(body->codegen().release())
        ));
        instructions.push_back(std::make_unique<assembly::Ret>());
        return std::make_unique<assembly::Function>(name, std::move(instructions));
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

    std::unique_ptr<assembly::AsmNode> codegen() const override {
        return std::make_unique<assembly::Program>(
                std::unique_ptr<assembly::Function>(
                        dynamic_cast<assembly::Function *>(function->codegen().release())
                )
        );
    }
};