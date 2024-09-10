//
// Created by vrin on 9/7/24.
//
#include "compiler_driver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

CompilerDriver::CompilerDriver()
        : m_lex_only(false), m_parse_only(false), m_codegen_only(false) {}

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
        m_output_file = m_input_file.substr(0, m_input_file.find_last_of('.'));
    }

    // Implement compilation stages
    if (m_lex_only) {
        return runLexer(m_input_file) ? 0 : 1;
    } else if (m_parse_only) {
        std::ifstream file(m_input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << m_input_file << std::endl;
            return 1;
        }
        std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Lexer lexer(input);
        std::vector<Token> tokens;
        try {
            tokens = lexer.tokenize();
        } catch (const std::exception& e) {
            std::cerr << "Lexer error: " << e.what() << std::endl;
            return 1;
        }
        return runParser(tokens) ? 0 : 1;
    } else if (m_codegen_only) {
        // Implement code generation (including lexer and parser)
    } else {
        // Full compilation process
        if (!preprocess(m_input_file, m_output_file + ".i") ||
            !compile(m_output_file + ".i", m_output_file + ".s") ||
            !assemble(m_output_file + ".s", m_output_file)) {
            return 1;
        }
    }

    return 0;
}

bool CompilerDriver::preprocess(const std::string& input_file, const std::string& output_file) {
    std::string command = "gcc -E -P " + input_file + " -o " + output_file;
    return system(command.c_str()) == 0;
}

bool CompilerDriver::compile(const std::string& input_file, const std::string& output_file) {
    // Implement your compiler logic here
    return true;
}

bool CompilerDriver::assemble(const std::string& input_file, const std::string& output_file) {
    std::string command = "gcc " + input_file + " -o " + output_file;
    return system(command.c_str()) == 0;
}

bool CompilerDriver::runLexer(const std::string& input_file) {
    // Read the input file
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << input_file << std::endl;
        return false;
    }

    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Create and run the lexer
    Lexer lexer(input);
    std::vector<Token> tokens;
    try {
        tokens = lexer.tokenize();
    } catch (const std::exception& e) {
        std::cerr << "Lexer error: " << e.what() << std::endl;
        return false;
    }

    // Output the tokens
    for (const auto& token : tokens) {
        std::cout << "Token: Type = ";
        switch (token.type) {
            case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case TokenType::CONSTANT: std::cout << "CONSTANT"; break;
            case TokenType::INT_KEYWORD: std::cout << "INT_KEYWORD"; break;
            case TokenType::VOID_KEYWORD: std::cout << "VOID_KEYWORD"; break;
            case TokenType::RETURN_KEYWORD: std::cout << "RETURN_KEYWORD"; break;
            case TokenType::OPEN_PAREN: std::cout << "OPEN_PAREN"; break;
            case TokenType::CLOSE_PAREN: std::cout << "CLOSE_PAREN"; break;
            case TokenType::OPEN_BRACE: std::cout << "OPEN_BRACE"; break;
            case TokenType::CLOSE_BRACE: std::cout << "CLOSE_BRACE"; break;
            case TokenType::SEMICOLON: std::cout << "SEMICOLON"; break;
        }
        std::cout << ", Value = \"" << token.value << "\"" << std::endl;
    }

    return true;
}

void CompilerDriver::printPrettyAST(const std::unique_ptr<Program>& ast) {
    std::cout << "Pretty-printed AST:\n" << ast->prettyPrint() << std::endl;
}

bool CompilerDriver::runParser(const std::vector<Token>& tokens) {
    try {
        Parser parser(tokens);
        std::unique_ptr<Program> ast = parser.parse();
        std::cout << "Parsing successful. AST created." << std::endl;
        printPrettyAST(ast);
        return true;
    } catch (const ParseError& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return false;
    }
}
void CompilerDriver::printUsage() {
    std::cout << "Usage: your_compiler [options] input_file [output_file]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --lex      Run only the lexer" << std::endl;
    std::cout << "  --parse    Run the lexer and parser" << std::endl;
    std::cout << "  --codegen  Run the lexer, parser, and code generation" << std::endl;
}