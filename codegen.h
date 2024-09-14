#pragma once

#include "ast.h"
#include "assembly_ast.h"

class CodeGen {
public:
    static std::unique_ptr<assembly::Program> generate(const Program &ast);

private:
    static std::unique_ptr<assembly::Function> generateFunction(const Function &function);

    static std::unique_ptr<assembly::AsmNode> generateStatement(const Statement &statement);

    static std::unique_ptr<assembly::Operand> generateExpression(const Exp &exp);
};