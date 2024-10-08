#include "compiler_driver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

CompilerDriver::CompilerDriver()
        : m_lex_only(false), m_parse_only(false), m_codegen_only(false), m_emit_assembly(false) {}

/**
 * @brief Runs the compiler driver with the given command line arguments.
 *
 * @details The compiler driver runs compilation stages up to the specified stage and writes the intermediate output
 * to the terminal, or (if no stage is specified) to a file. The command line args are the stage flags and the input file name.
 *
 * @param argc
 * @param argv
 * @return
 */
int CompilerDriver::run(int argc, char *argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--lex") {
            m_lex_only = true;
        } else if (arg == "--parse") {
            m_parse_only = true;
        } else if (arg == "--codegen") {
            m_codegen_only = true;
        } else if (arg == "-S") {
            m_emit_assembly = true;
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

    if (m_emit_assembly) {
        // If -S flag is set, we stop here and keep the .s file
        std::cout << "Assembly code generated and written to " << assembly_file << std::endl;
        std::remove(preprocessed_file.c_str());
        return 0;
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

    // std::cout << "Compilation completed successfully. Output written to " << m_output_file << std::endl;
    return 0;
}

/**
 * @brief Preprocesses the input file using the GCC preprocessor.
 *
 * @param input_file The path to the input file to be preprocessed.
 * @param output_file The path to the output file where the preprocessed content will be written.
 * @returns `true` if preprocessing is successful, `false` otherwise.
 */
bool CompilerDriver::preprocess(const std::string &input_file, const std::string &output_file) {
    std::string command = "gcc -E -P " + input_file + " -o " + output_file;
    return system(command.c_str()) == 0;
}

/**
 * @brief Assembles the given assembly file using GCC.
 *
 * @param input_file The path to the input assembly file.
 * @param output_file The path to the output file where the executable will be written.
 * @returns `true` if assembly is successful, `false` otherwise.
 */
bool CompilerDriver::assemble(const std::string &input_file, const std::string &output_file) {
    std::string command = "gcc " + input_file + " -o " + output_file;
    return system(command.c_str()) == 0;
}

/**
 * @brief Runs the lexer on the input file.
 *
 * @details The lexer reads the input file, tokenizes it, and stores the tokens in the given vector.
 *
 * @param input_file The path to the input file to be tokenized.
 * @param tokens The vector where the tokens will be stored.
 * @returns `true` if the lexer runs successfully, `false` otherwise.
 */
bool CompilerDriver::runLexer(const std::string &input_file, std::vector<Token> &tokens) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << input_file << std::endl;
        return false;
    }

    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Lexer lexer(input);
    try {
        tokens = lexer.tokenize();
    } catch (const std::exception &e) {
        std::cerr << "Lexer error: " << e.what() << std::endl;
        return false;
    }

    if (m_lex_only) {
        // Output the tokens
        for (const auto &token: tokens) {
            std::cout << "Token: Type = " << static_cast<int>(token.type)
                      << ", Value = \"" << token.value << "\"" << std::endl;
        }
    }

    return true;
}

/**
 * @brief Runs the parser on the given tokens.
 *
 * @details The parser reads the tokens, constructs an abstract syntax tree (AST), and stores it in the given pointer.
 *
 * @param tokens The tokens to be parsed.
 * @param ast The pointer where the AST will be stored.
 * @returns `true` if the parser runs successfully, `false` otherwise.
 */
bool CompilerDriver::runParser(const std::vector<Token> &tokens, std::unique_ptr<Program> &ast) {
    try {
        Parser parser(tokens);
        ast = parser.parse();
        if (m_parse_only) {
            std::cout << "Parsing successful. AST created." << std::endl;
            printPrettyAST(ast);
        }
        return true;
    } catch (const ParseError &e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Runs the code generation stage on the given AST.
 *
 * @details The code generation stage reads the AST, generates an assembly AST, and stores it in the given pointer.
 *
 * @param ast The AST to generate code from.
 * @param asmProgram The pointer where the assembly AST will be stored.
 * @returns `true` if code generation is successful, `false` otherwise.
 */
bool CompilerDriver::runCodeGen(const std::unique_ptr<Program> &ast, std::unique_ptr<assembly::Program> &asmProgram) {
    try {
        asmProgram = CodeGen::generate(*ast);
        if (m_codegen_only) {
            std::cout << "Code generation successful. Assembly AST created." << std::endl;
            printPrettyAssemblyAST(asmProgram);
        }
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Code generation error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Emits the generated assembly code to the output file.
 *
 * @param asmProgram The assembly AST to emit.
 * @param output_file The path to the output file where the assembly code will be written.
 * @returns `true` if the code is emitted successfully, `false` otherwise.
 */
bool CompilerDriver::emitCode(const std::unique_ptr<assembly::Program> &asmProgram, const std::string &output_file) {
    std::ofstream outFile(output_file);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open output file " << output_file << std::endl;
        return false;
    }
    outFile << asmProgram->emit();
    // std::cout << "Assembly code generated and written to " << output_file << std::endl;
    return true;
}

void CompilerDriver::printPrettyAST(const std::unique_ptr<Program> &ast) {
    std::cout << "Pretty-printed AST:\n" << ast->prettyPrint() << std::endl;
}

void CompilerDriver::printPrettyAssemblyAST(const std::unique_ptr<assembly::Program> &asmProgram) {
    std::cout << "Pretty-printed Assembly AST:\n" << asmProgram->prettyPrint() << std::endl;
}

void CompilerDriver::printUsage() {
    std::cout << "Usage: your_compiler [options] input_file [output_file]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --lex      Run only the lexer" << std::endl;
    std::cout << "  --parse    Run the lexer and parser" << std::endl;
    std::cout << "  --codegen  Run the lexer, parser, and code generation" << std::endl;
    std::cout << "  -S         Emit assembly code only" << std::endl;
}