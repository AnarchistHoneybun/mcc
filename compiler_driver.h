#pragma once
#include <string>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "assembly_ast.h"

class CompilerDriver {
public:
    CompilerDriver();
    int run(int argc, char* argv[]);

private:
    bool preprocess(const std::string& input_file, const std::string& output_file);
    bool compile(const std::string& input_file, const std::string& output_file);
    bool assemble(const std::string& input_file, const std::string& output_file);
    bool runLexer(const std::string& input_file);
    bool runParser(const std::vector<Token>& tokens);
    void printPrettyAST(const std::unique_ptr<Program>& ast);
    bool generateCode(const std::unique_ptr<Program>& ast, const std::string& output_file);
    void printUsage();

    std::string m_input_file;
    std::string m_output_file;
    bool m_lex_only;
    bool m_parse_only;
    bool m_codegen_only;
};