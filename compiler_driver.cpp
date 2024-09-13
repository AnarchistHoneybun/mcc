#include "compiler_driver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

CompilerDriver::CompilerDriver()
        : m_lex_only(false), m_parse_only(false), m_codegen_only(false) {}

// int CompilerDriver::run(int argc, char* argv[]) {
//     // Parse command line arguments
//     for (int i = 1; i < argc; ++i) {
//         std::string arg = argv[i];
//         if (arg == "--lex") {
//             m_lex_only = true;
//         } else if (arg == "--parse") {
//             m_parse_only = true;
//         } else if (arg == "--codegen") {
//             m_codegen_only = true;
//         } else if (arg[0] == '-') {
//             std::cerr << "Unknown option: " << arg << std::endl;
//             printUsage();
//             return 1;
//         } else {
//             if (m_input_file.empty()) {
//                 m_input_file = arg;
//             } else if (m_output_file.empty()) {
//                 m_output_file = arg;
//             } else {
//                 std::cerr << "Too many arguments" << std::endl;
//                 printUsage();
//                 return 1;
//             }
//         }
//     }

//     if (m_input_file.empty()) {
//         std::cerr << "No input file specified" << std::endl;
//         printUsage();
//         return 1;
//     }

//     if (m_output_file.empty()) {
//         m_output_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + ".s";
//     }

//     // Run compilation stages
//     std::vector<Token> tokens;
//     std::unique_ptr<Program> ast;
//     std::unique_ptr<assembly::Program> asmProgram;

//     // Lexer stage
//     if (!runLexer(m_input_file, tokens)) {
//         return 1;
//     }
//     if (m_lex_only) return 0;

//     // Parser stage
//     if (!runParser(tokens, ast)) {
//         return 1;
//     }
//     if (m_parse_only) return 0;

//     // Code generation stage
//     if (!runCodeGen(ast, asmProgram)) {
//         return 1;
//     }
//     if (m_codegen_only) return 0;

//     // Code emission stage
//     if (!emitCode(asmProgram, m_output_file)) {
//         return 1;
//     }

//     std::cout << "Compilation completed successfully. Output written to " << m_output_file << std::endl;
//     return 0;
// }

// int CompilerDriver::run(int argc, char* argv[]) {
//     // Parse command line arguments
//     for (int i = 1; i < argc; ++i) {
//         std::string arg = argv[i];
//         if (arg == "--lex") {
//             m_lex_only = true;
//         } else if (arg == "--parse") {
//             m_parse_only = true;
//         } else if (arg == "--codegen") {
//             m_codegen_only = true;
//         } else if (arg[0] == '-') {
//             std::cerr << "Unknown option: " << arg << std::endl;
//             printUsage();
//             return 1;
//         } else {
//             if (m_input_file.empty()) {
//                 m_input_file = arg;
//             } else if (m_output_file.empty()) {
//                 m_output_file = arg;
//             } else {
//                 std::cerr << "Too many arguments" << std::endl;
//                 printUsage();
//                 return 1;
//             }
//         }
//     }

//     if (m_input_file.empty()) {
//         std::cerr << "No input file specified" << std::endl;
//         printUsage();
//         return 1;
//     }

//     if (m_output_file.empty()) {
//         m_output_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + "";
//     }

//     // Preprocess
//     std::string preprocessed_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + ".i";
//     if (!preprocess(m_input_file, preprocessed_file)) {
//         std::cerr << "Preprocessing failed" << std::endl;
//         return 1;
//     }

//     // Run compilation stages
//     std::vector<Token> tokens;
//     std::unique_ptr<Program> ast;
//     std::unique_ptr<assembly::Program> asmProgram;

//     // Lexer stage
//     if (!runLexer(preprocessed_file, tokens)) {
//         return 1;
//     }
//     if (m_lex_only) return 0;

//     // Parser stage
//     if (!runParser(tokens, ast)) {
//         return 1;
//     }
//     if (m_parse_only) return 0;

//     // Code generation stage
//     if (!runCodeGen(ast, asmProgram)) {
//         return 1;
//     }
//     if (m_codegen_only) return 0;

//     // Code emission stage
//     std::string assembly_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + ".s";
//     if (!emitCode(asmProgram, assembly_file)) {
//         return 1;
//     }

//     // Assemble
//     if (!assemble(assembly_file, m_output_file)) {
//         std::cerr << "Assembly failed" << std::endl;
//         return 1;
//     }

//     std::cout << "Compilation completed successfully. Output written to " << m_output_file << std::endl;
//     return 0;
// }

int CompilerDriver::run(int argc, char* argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--lex") {
            m_lex_only = true;
        } else if (arg == "--parse") {
            m_parse_only = true;
        } else if (arg == "--codegen") {
            m_codegen_only = true;
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        } else {
            if (m_input_file.empty()) {
                m_input_file = arg;
            } else if (m_output_file.empty()) {
                m_output_file = arg;
            } else {
                std::cerr << "Too many arguments" << std::endl;
                printUsage();
                return 1;
            }
        }
    }

    if (m_input_file.empty()) {
        std::cerr << "No input file specified" << std::endl;
        printUsage();
        return 1;
    }

    if (m_output_file.empty()) {
        m_output_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + "";
    }

    // Preprocess
    std::string preprocessed_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + ".i";
    if (!preprocess(m_input_file, preprocessed_file)) {
        std::cerr << "Preprocessing failed" << std::endl;
        return 1;
    }

    // Run compilation stages
    std::vector<Token> tokens;
    std::unique_ptr<Program> ast;
    std::unique_ptr<assembly::Program> asmProgram;

    // Lexer stage
    if (!runLexer(preprocessed_file, tokens)) {
        std::remove(preprocessed_file.c_str());
        return 1;
    }
    if (m_lex_only) {
        std::remove(preprocessed_file.c_str());
        return 0;
    }

    // Parser stage
    if (!runParser(tokens, ast)) {
        std::remove(preprocessed_file.c_str());
        return 1;
    }
    if (m_parse_only) {
        std::remove(preprocessed_file.c_str());
        return 0;
    }

    // Code generation stage
    if (!runCodeGen(ast, asmProgram)) {
        std::remove(preprocessed_file.c_str());
        return 1;
    }
    if (m_codegen_only) {
        std::remove(preprocessed_file.c_str());
        return 0;
    }

    // Code emission stage
    std::string assembly_file = m_input_file.substr(0, m_input_file.find_last_of('.')) + ".s";
    if (!emitCode(asmProgram, assembly_file)) {
        std::remove(preprocessed_file.c_str());
        return 1;
    }

    // Assemble
    if (!assemble(assembly_file, m_output_file)) {
        std::cerr << "Assembly failed" << std::endl;
        std::remove(preprocessed_file.c_str());
        std::remove(assembly_file.c_str());
        return 1;
    }

    // Cleanup intermediate files
    std::remove(preprocessed_file.c_str());
    std::remove(assembly_file.c_str());

    std::cout << "Compilation completed successfully. Output written to " << m_output_file << std::endl;
    return 0;
}

bool CompilerDriver::preprocess(const std::string& input_file, const std::string& output_file) {
    std::string command = "gcc -E -P " + input_file + " -o " + output_file;
    return system(command.c_str()) == 0;
}

bool CompilerDriver::assemble(const std::string& input_file, const std::string& output_file) {
    std::string command = "gcc " + input_file + " -o " + output_file;
    return system(command.c_str()) == 0;
}


bool CompilerDriver::runLexer(const std::string& input_file, std::vector<Token>& tokens) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << input_file << std::endl;
        return false;
    }

    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Lexer lexer(input);
    try {
        tokens = lexer.tokenize();
    } catch (const std::exception& e) {
        std::cerr << "Lexer error: " << e.what() << std::endl;
        return false;
    }

    if (m_lex_only) {
        // Output the tokens
        for (const auto& token : tokens) {
            std::cout << "Token: Type = " << static_cast<int>(token.type)
                      << ", Value = \"" << token.value << "\"" << std::endl;
        }
    }

    return true;
}

bool CompilerDriver::runParser(const std::vector<Token>& tokens, std::unique_ptr<Program>& ast) {
    try {
        Parser parser(tokens);
        ast = parser.parse();
        if (m_parse_only) {
            std::cout << "Parsing successful. AST created." << std::endl;
            printPrettyAST(ast);
        }
        return true;
    } catch (const ParseError& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool CompilerDriver::runCodeGen(const std::unique_ptr<Program>& ast, std::unique_ptr<assembly::Program>& asmProgram) {
    try {
        asmProgram = CodeGen::generate(*ast);
        if (m_codegen_only) {
            std::cout << "Code generation successful. Assembly AST created." << std::endl;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Code generation error: " << e.what() << std::endl;
        return false;
    }
}

bool CompilerDriver::emitCode(const std::unique_ptr<assembly::Program>& asmProgram, const std::string& output_file) {
    std::ofstream outFile(output_file);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open output file " << output_file << std::endl;
        return false;
    }
    outFile << asmProgram->emit();
    std::cout << "Assembly code generated and written to " << output_file << std::endl;
    return true;
}

void CompilerDriver::printPrettyAST(const std::unique_ptr<Program>& ast) {
    std::cout << "Pretty-printed AST:\n" << ast->prettyPrint() << std::endl;
}

void CompilerDriver::printUsage() {
    std::cout << "Usage: your_compiler [options] input_file [output_file]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --lex      Run only the lexer" << std::endl;
    std::cout << "  --parse    Run the lexer and parser" << std::endl;
    std::cout << "  --codegen  Run the lexer, parser, and code generation" << std::endl;
}