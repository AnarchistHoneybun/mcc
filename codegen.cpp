#include "codegen.h"

/**
 * @brief Generates an assembly program from the given abstract syntax tree (AST).
 *
 * @param ast The abstract syntax tree representing the program.
 * @return A unique pointer to the generated assembly program.
 */
std::unique_ptr<assembly::Program> CodeGen::generate(const Program &ast) {
    return std::make_unique<assembly::Program>(generateFunction(*ast.function));
}

/**
 * @brief Generates an assembly function from the given function AST node.
 *
 * @param function The function AST node.
 * @return A unique pointer to the generated assembly function.
 */
std::unique_ptr<assembly::Function> CodeGen::generateFunction(const Function &function) {
    std::vector<std::unique_ptr<assembly::Instruction>> instructions;
    instructions.push_back(std::unique_ptr<assembly::Instruction>(
            dynamic_cast<assembly::Instruction *>(generateStatement(*function.body).release())
    ));
    instructions.push_back(std::make_unique<assembly::Ret>());
    return std::make_unique<assembly::Function>(function.name, std::move(instructions));
}

/**
 * @brief Generates an assembly statement from the given statement AST node.
 *
 * @param statement The statement AST node.
 * @return A unique pointer to the generated assembly statement.
 */
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

/**
 * @brief Generates an assembly operand from the given expression AST node.
 *
 * @param exp The expression AST node.
 * @return A unique pointer to the generated assembly operand.
 */
std::unique_ptr<assembly::Operand> CodeGen::generateExpression(const Exp &exp) {
    if (const auto *constant = dynamic_cast<const Constant *>(&exp)) {
        return std::make_unique<assembly::Imm>(constant->value);
    }
    throw std::runtime_error("Unsupported expression type");
}