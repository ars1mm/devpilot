#pragma once

#include <string>
#include <vector>

namespace devpilot {

enum class SymbolType {
    FUNCTION,
    CLASS,
    VARIABLE,
    NAMESPACE,
    UNKNOWN
};

struct Symbol {
    std::string name;
    SymbolType type;
    std::string file_path;
    int line_number;
    int column_number;
    std::string signature;
    std::string parent_scope;  // For methods inside classes
    
    Symbol() = default;
    Symbol(const std::string& name, SymbolType type, const std::string& file_path,
           int line, int column, const std::string& signature = "")
        : name(name), type(type), file_path(file_path), 
          line_number(line), column_number(column), signature(signature) {}
    
    bool operator==(const Symbol& other) const {
        return name == other.name && 
               type == other.type && 
               file_path == other.file_path &&
               line_number == other.line_number;
    }
};

// Convert SymbolType to string for display
std::string symbolTypeToString(SymbolType type);

// Convert string to SymbolType for storage
SymbolType stringToSymbolType(const std::string& typeStr);

class SymbolManager {
public:
    void addSymbol(const Symbol& symbol);
    std::vector<Symbol> findSymbols(const std::string& name);
    std::vector<Symbol> getSymbolsInFile(const std::string& filePath);
    std::vector<Symbol> getAllSymbols() const;
    void clear();
    
private:
    std::vector<Symbol> symbols;
};

} // namespace devpilot
