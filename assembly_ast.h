//
// Created by vrin on 9/11/24.
//

#ifndef MCC_ASSEMBLY_AST_H
#define MCC_ASSEMBLY_AST_H

#endif //MCC_ASSEMBLY_AST_H

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace assembly {

    class AsmNode {
    public:
        virtual ~AsmNode() = default;

        virtual std::string emit() const = 0;

        virtual std::string prettyPrint(int indent = 0) const = 0;

    protected:
        static std::string indentString(int indent) {
            return std::string(indent * 2, ' ');
        }
    };

    class Operand : public AsmNode {
    public:
        virtual ~Operand() = default;
    };

    class Imm : public Operand {
    public:
        explicit Imm(int value) : value(value) {}

        std::string emit() const override {
            return "$" + std::to_string(value);
        }

        std::string prettyPrint(int indent = 0) const override {
            return indentString(indent) + "Imm(" + std::to_string(value) + ")";
        }

        int value;
    };

    class Register : public Operand {
    public:
        explicit Register(const std::string &name) : name(name) {}

        std::string emit() const override {
            return "%" + name;
        }

        std::string prettyPrint(int indent = 0) const override {
            return indentString(indent) + "Register(\"" + name + "\")";
        }

        std::string name;
    };

    class Instruction : public AsmNode {
    public:
        virtual ~Instruction() = default;
    };

    class Mov : public Instruction {
    public:
        Mov(std::unique_ptr<Operand> src, std::unique_ptr<Operand> dst)
                : src(std::move(src)), dst(std::move(dst)) {}

        std::string emit() const override {
            return "movl " + src->emit() + ", " + dst->emit();
        }

        std::string prettyPrint(int indent = 0) const override {
            std::ostringstream oss;
            oss << indentString(indent) << "Mov(\n"
                << src->prettyPrint(indent + 1) << ",\n"
                << dst->prettyPrint(indent + 1) << "\n"
                << indentString(indent) << ")";
            return oss.str();
        }

        std::unique_ptr<Operand> src;
        std::unique_ptr<Operand> dst;
    };

    class Ret : public Instruction {
    public:
        std::string emit() const override {
            return "ret";
        }

        std::string prettyPrint(int indent = 0) const override {
            return indentString(indent) + "Ret()";
        }
    };

    class Function : public AsmNode {
    public:
        Function(const std::string &name, std::vector<std::unique_ptr<Instruction>> instructions)
                : name(name), instructions(std::move(instructions)) {}

        std::string emit() const override {
            std::ostringstream oss;
            oss << ".globl " << name << "\n";
            oss << name << ":\n";
            for (const auto &instruction: instructions) {
                oss << "    " << instruction->emit() << "\n";
            }
            return oss.str();
        }

        std::string prettyPrint(int indent = 0) const override {
            std::ostringstream oss;
            oss << indentString(indent) << "Function(\n"
                << indentString(indent + 1) << "name=\"" << name << "\",\n"
                << indentString(indent + 1) << "instructions=[\n";
            for (const auto &instruction: instructions) {
                oss << instruction->prettyPrint(indent + 2) << ",\n";
            }
            oss << indentString(indent + 1) << "]\n"
                << indentString(indent) << ")";
            return oss.str();
        }

        std::string name;
        std::vector<std::unique_ptr<Instruction>> instructions;
    };

    class Program : public AsmNode {
    public:
        explicit Program(std::unique_ptr<Function> function)
                : function(std::move(function)) {}

        std::string emit() const override {
            std::ostringstream oss;
            oss << function->emit();
            oss << "\n.section .note.GNU-stack,\"\",@progbits\n";
            return oss.str();
        }

        std::string prettyPrint(int indent = 0) const override {
            std::ostringstream oss;
            oss << indentString(indent) << "Program(\n"
                << function->prettyPrint(indent + 1) << "\n"
                << indentString(indent) << ")";
            return oss.str();
        }

        std::unique_ptr<Function> function;
    };

} // namespace assembly