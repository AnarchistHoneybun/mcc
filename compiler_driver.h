#pragma once

#include <string>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "assembly_ast.h"
#include "codegen.h"

class CompilerDriver {
public:
    CompilerDriver();

    int run(int argc, char *argv[]);

private:
    bool preprocess(const std::string &input_file, const std::string &output_file);

    bool runLexer(const std::string &input_file, std::vector<Token> &tokens);

    bool runParser(const std::vector<Token> &tokens, std::unique_ptr<Program> &ast);

    bool runCodeGen(const std::unique_ptr<Program> &ast, std::unique_ptr<assembly::Program> &asmProgram);

    bool emitCode(const std::unique_ptr<assembly::Program> &asmProgram, const std::string &output_file);

    bool assemble(const std::string &input_file, const std::string &output_file);

    void printPrettyAST(const std::unique_ptr<Program> &ast);

    void printPrettyAssemblyAST(const std::unique_ptr<assembly::Program> &asmProgram);

    void printUsage();

    std::string m_input_file;
    std::string m_output_file;
    bool m_lex_only;
    bool m_parse_only;
    bool m_codegen_only;
    bool m_emit_assembly;
};