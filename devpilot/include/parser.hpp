#pragma once

#include "symbol.hpp"
#include <string>
#include <vector>

// Forward declarations for TreeSitter
typedef struct TSParser TSParser;
typedef struct TSLanguage TSLanguage;
typedef struct TSNode TSNode;

namespace devpilot {

class CppParser {
public:
    CppParser();
    ~CppParser();
    
    // Single responsibility: Only parse C++ files using TreeSitter
    std::vector<Symbol> parseFile(const std::string& filePath);
    
    // Check if parser is properly initialized
    bool isInitialized() const;
    
private:
    TSParser* parser;
    const TSLanguage* cpp_language;
    bool initialized;
    
    // Fallback parser for when TreeSitter C++ grammar is not available
    std::vector<Symbol> parseWithFallback(const std::string& source, const std::string& filePath);
    void extractFunctionsFromText(const std::string& source, const std::string& filePath, std::vector<Symbol>& symbols);
    void extractClassesFromText(const std::string& source, const std::string& filePath, std::vector<Symbol>& symbols);
    
#ifdef HAVE_TREE_SITTER
    // TreeSitter parsing methods
    void extractSymbols(TSNode node, const std::string& source, 
                       const std::string& filePath, std::vector<Symbol>& symbols);
    void extractFunctions(TSNode node, const std::string& source,
                         const std::string& filePath, std::vector<Symbol>& symbols);
    void extractClasses(TSNode node, const std::string& source,
                       const std::string& filePath, std::vector<Symbol>& symbols);
    void extractVariables(TSNode node, const std::string& source,
                         const std::string& filePath, std::vector<Symbol>& symbols);
    
    // Helper methods for TreeSitter
    std::string getNodeText(TSNode node, const std::string& source);
    std::string getNodeType(TSNode node);
    bool isNodeType(TSNode node, const std::string& type);
#endif
    
    // Read file contents
    std::string readFile(const std::string& filePath);
};

} // namespace devpilot
