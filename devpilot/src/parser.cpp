#include "parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cctype>

#ifdef HAVE_TREE_SITTER
// TreeSitter includes
extern "C" {
#include <tree_sitter/api.h>
// Note: We'll implement a fallback since tree-sitter-cpp might not be available
// extern const TSLanguage *tree_sitter_cpp(void);
}
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#ifdef HAVE_TREE_SITTER
// TreeSitter includes
extern "C" {
#include <tree_sitter/api.h>
// Note: We'll implement a fallback since tree-sitter-cpp might not be available
// extern const TSLanguage *tree_sitter_cpp(void);
}
#endif

namespace devpilot {

// Single responsibility: Only parse C++ files using TreeSitter

CppParser::CppParser() : parser(nullptr), cpp_language(nullptr), initialized(false) {
#ifdef HAVE_TREE_SITTER
    parser = ts_parser_new();
    // For now, we'll use C grammar as a fallback since C++ grammar might not be available
    // cpp_language = tree_sitter_cpp();
    
    if (parser) {
        // We'll implement a simple fallback parser for now
        initialized = true;
        std::cout << "Parser initialized (TreeSitter available but using fallback implementation)" << std::endl;
    }
#else
    // Fallback implementation without TreeSitter
    initialized = true;
    std::cout << "Parser initialized (fallback implementation - limited functionality)" << std::endl;
#endif
    
    if (!initialized) {
        std::cerr << "Failed to initialize C++ parser" << std::endl;
    }
}

CppParser::~CppParser() {
#ifdef HAVE_TREE_SITTER
    if (parser) {
        ts_parser_delete(parser);
    }
#endif
}

bool CppParser::isInitialized() const {
    return initialized;
}

std::vector<Symbol> CppParser::parseFile(const std::string& filePath) {
    std::vector<Symbol> symbols;
    
    if (!initialized) {
        std::cerr << "Parser not initialized" << std::endl;
        return symbols;
    }
    
    // Read file contents
    std::string source = readFile(filePath);
    if (source.empty()) {
        std::cerr << "Could not read file: " << filePath << std::endl;
        return symbols;
    }
    
#ifdef HAVE_TREE_SITTER
    // For now, we'll implement a simple fallback even with TreeSitter available
    // since we don't have the C++ grammar installed
    std::cout << "Using fallback parser for: " << filePath << std::endl;
    return parseWithFallback(source, filePath);
#else
    // Fallback implementation without TreeSitter
    std::cout << "Using fallback parser for: " << filePath << std::endl;
    return parseWithFallback(source, filePath);
#endif
}

// Fallback parser implementation using simple text processing
std::vector<Symbol> CppParser::parseWithFallback(const std::string& source, const std::string& filePath) {
    std::vector<Symbol> symbols;
    
    // Simple text-based parsing - not as accurate as TreeSitter but functional for MVP
    extractFunctionsFromText(source, filePath, symbols);
    extractClassesFromText(source, filePath, symbols);
    
    return symbols;
}

void CppParser::extractFunctionsFromText(const std::string& source, const std::string& filePath, std::vector<Symbol>& symbols) {
    std::istringstream stream(source);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // Look for function patterns: "type functionName(" 
        // This is a simple heuristic - not perfect but functional
        size_t parenPos = line.find('(');
        if (parenPos != std::string::npos) {
            // Skip lines that look like function calls (inside other functions)
            if (line.find("    ") == 0 || line.find("\t") == 0) {
                continue; // Skip indented lines (likely function calls)
            }
            
            // Skip preprocessor directives and comments
            if (line.find("#") != std::string::npos || 
                line.find("//") != std::string::npos ||
                line.find("/*") != std::string::npos) {
                continue;
            }
            
            // Extract potential function name
            std::string beforeParen = line.substr(0, parenPos);
            
            // Find the last word before the parenthesis
            size_t lastSpace = beforeParen.find_last_of(" \t");
            if (lastSpace != std::string::npos) {
                std::string functionName = beforeParen.substr(lastSpace + 1);
                
                // Clean up the function name
                size_t asteriskPos = functionName.find('*');
                if (asteriskPos != std::string::npos) {
                    functionName = functionName.substr(asteriskPos + 1);
                }
                
                // Skip constructors/destructors for now, and invalid names
                if (!functionName.empty() && 
                    functionName.find("if") != 0 && 
                    functionName.find("while") != 0 &&
                    functionName.find("for") != 0 &&
                    functionName != "return" &&
                    std::isalpha(functionName[0])) {
                    
                    std::string signature = line;
                    if (signature.length() > 100) {
                        signature = signature.substr(0, 97) + "...";
                    }
                    
                    Symbol symbol(functionName, SymbolType::FUNCTION, filePath, lineNumber, 0, signature);
                    symbols.push_back(symbol);
                }
            }
        }
    }
}

void CppParser::extractClassesFromText(const std::string& source, const std::string& filePath, std::vector<Symbol>& symbols) {
    std::istringstream stream(source);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // Look for class declarations: "class ClassName"
        if (line.find("class ") != std::string::npos || line.find("struct ") != std::string::npos) {
            // Skip comments
            if (line.find("//") != std::string::npos || line.find("/*") != std::string::npos) {
                continue;
            }
            
            size_t classPos = line.find("class ");
            size_t structPos = line.find("struct ");
            size_t keywordPos = (classPos != std::string::npos) ? classPos : structPos;
            
            if (keywordPos != std::string::npos) {
                std::string keyword = (classPos != std::string::npos) ? "class " : "struct ";
                size_t nameStart = keywordPos + keyword.length();
                
                // Extract class name
                std::string rest = line.substr(nameStart);
                size_t spacePos = rest.find_first_of(" \t{:");
                std::string className = (spacePos != std::string::npos) ? rest.substr(0, spacePos) : rest;
                
                if (!className.empty() && std::isalpha(className[0])) {
                    std::string signature = keyword + className;
                    Symbol symbol(className, SymbolType::CLASS, filePath, lineNumber, 0, signature);
                    symbols.push_back(symbol);
                }
            }
        }
    }
}

#ifdef HAVE_TREE_SITTER

void CppParser::extractSymbols(TSNode node, const std::string& source, 
                              const std::string& filePath, std::vector<Symbol>& symbols) {
    // Extract different types of symbols
    extractFunctions(node, source, filePath, symbols);
    extractClasses(node, source, filePath, symbols);
    extractVariables(node, source, filePath, symbols);
    
    // Recursively process child nodes
    uint32_t childCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < childCount; i++) {
        TSNode child = ts_node_child(node, i);
        extractSymbols(child, source, filePath, symbols);
    }
}

void CppParser::extractFunctions(TSNode node, const std::string& source,
                                const std::string& filePath, std::vector<Symbol>& symbols) {
    if (isNodeType(node, "function_definition") || isNodeType(node, "function_declarator")) {
        // Find function name
        TSNode nameNode = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));
        if (ts_node_is_null(nameNode)) {
            nameNode = ts_node_child_by_field_name(node, "name", strlen("name"));
        }
        
        if (!ts_node_is_null(nameNode)) {
            std::string name = getNodeText(nameNode, source);
            if (!name.empty()) {
                TSPoint startPoint = ts_node_start_point(node);
                std::string signature = getNodeText(node, source);
                
                // Limit signature length for storage
                if (signature.length() > 200) {
                    signature = signature.substr(0, 197) + "...";
                }
                
                Symbol symbol(name, SymbolType::FUNCTION, filePath, 
                            startPoint.row + 1, startPoint.column + 1, signature);
                symbols.push_back(symbol);
            }
        }
    }
}

void CppParser::extractClasses(TSNode node, const std::string& source,
                              const std::string& filePath, std::vector<Symbol>& symbols) {
    if (isNodeType(node, "class_specifier") || isNodeType(node, "struct_specifier")) {
        TSNode nameNode = ts_node_child_by_field_name(node, "name", strlen("name"));
        
        if (!ts_node_is_null(nameNode)) {
            std::string name = getNodeText(nameNode, source);
            if (!name.empty()) {
                TSPoint startPoint = ts_node_start_point(node);
                std::string signature = "class " + name;
                
                Symbol symbol(name, SymbolType::CLASS, filePath,
                            startPoint.row + 1, startPoint.column + 1, signature);
                symbols.push_back(symbol);
            }
        }
    }
}

void CppParser::extractVariables(TSNode node, const std::string& source,
                                const std::string& filePath, std::vector<Symbol>& symbols) {
    if (isNodeType(node, "declaration")) {
        // This is a simplified variable extraction
        // In a full implementation, we'd handle different variable declaration patterns
        TSNode declaratorNode = ts_node_child_by_field_name(node, "declarator", strlen("declarator"));
        
        if (!ts_node_is_null(declaratorNode)) {
            std::string name = getNodeText(declaratorNode, source);
            if (!name.empty() && name.find('(') == std::string::npos) { // Not a function
                TSPoint startPoint = ts_node_start_point(node);
                std::string signature = getNodeText(node, source);
                
                if (signature.length() > 100) {
                    signature = signature.substr(0, 97) + "...";
                }
                
                Symbol symbol(name, SymbolType::VARIABLE, filePath,
                            startPoint.row + 1, startPoint.column + 1, signature);
                symbols.push_back(symbol);
            }
        }
    }
}

std::string CppParser::getNodeText(TSNode node, const std::string& source) {
    if (ts_node_is_null(node)) {
        return "";
    }
    
    uint32_t startByte = ts_node_start_byte(node);
    uint32_t endByte = ts_node_end_byte(node);
    
    if (startByte >= source.length() || endByte > source.length() || startByte >= endByte) {
        return "";
    }
    
    return source.substr(startByte, endByte - startByte);
}

std::string CppParser::getNodeType(TSNode node) {
    if (ts_node_is_null(node)) {
        return "";
    }
    
    const char* type = ts_node_type(node);
    return type ? std::string(type) : "";
}

bool CppParser::isNodeType(TSNode node, const std::string& type) {
    return getNodeType(node) == type;
}
#endif

std::string CppParser::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace devpilot
