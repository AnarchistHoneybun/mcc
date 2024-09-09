//
// Created by vrin on 9/7/24.
//

#ifndef MCC_COMPILER_DRIVER_H
#define MCC_COMPILER_DRIVER_H

#endif //MCC_COMPILER_DRIVER_H

#pragma once
#include <string>
#include "lexer.h"

class CompilerDriver {
public:
    CompilerDriver();
    int run(int argc, char* argv[]);

private:
    bool preprocess(const std::string& input_file, const std::string& output_file);
    bool compile(const std::string& input_file, const std::string& output_file);
    bool assemble(const std::string& input_file, const std::string& output_file);
    bool runLexer(const std::string& input_file);
    void printUsage();

    std::string m_input_file;
    std::string m_output_file;
    bool m_lex_only;
    bool m_parse_only;
    bool m_codegen_only;
};