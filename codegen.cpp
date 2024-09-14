#include "codegen.h"

std::unique_ptr<assembly::Program> CodeGen::generate(const Program &ast) {
    return std::make_unique<assembly::Program>(generateFunction(*ast.function));
}

std::unique_ptr<assembly::Function> CodeGen::generateFunction(const Function &function) {
    std::vector<std::unique_ptr<assembly::Instruction>> instructions;
    instructions.push_back(std::unique_ptr<assembly::Instruction>(
            dynamic_cast<assembly::Instruction *>(generateStatement(*function.body).release())
    ));
    instructions.push_back(std::make_unique<assembly::Ret>());
    return std::make_unique<assembly::Function>(function.name, std::move(instructions));
}

std::unique_ptr<assembly::AsmNode> CodeGen::generateStatement(const Statement &statement) {
    if (const auto *returnStmt = dynamic_cast<const Return *>(&statement)) {
        auto expCode = generateExpression(*returnStmt->exp);
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
    throw std::runtime_error("Unsupported statement type");
}

std::unique_ptr<assembly::Operand> CodeGen::generateExpression(const Exp &exp) {
    if (const auto *constant = dynamic_cast<const Constant *>(&exp)) {
        return std::make_unique<assembly::Imm>(constant->value);
    }
    throw std::runtime_error("Unsupported expression type");
}