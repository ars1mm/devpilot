#include "symbol.hpp"
#include <algorithm>

namespace devpilot {

// Single responsibility: Only handle symbol data structures and operations

std::string symbolTypeToString(SymbolType type) {
    switch (type) {
        case SymbolType::FUNCTION: return "function";
        case SymbolType::CLASS: return "class";
        case SymbolType::VARIABLE: return "variable";
        case SymbolType::NAMESPACE: return "namespace";
        case SymbolType::UNKNOWN: return "unknown";
    }
    return "unknown";
}

SymbolType stringToSymbolType(const std::string& typeStr) {
    if (typeStr == "function") return SymbolType::FUNCTION;
    if (typeStr == "class") return SymbolType::CLASS;
    if (typeStr == "variable") return SymbolType::VARIABLE;
    if (typeStr == "namespace") return SymbolType::NAMESPACE;
    return SymbolType::UNKNOWN;
}

void SymbolManager::addSymbol(const Symbol& symbol) {
    // Avoid duplicates
    auto it = std::find(symbols.begin(), symbols.end(), symbol);
    if (it == symbols.end()) {
        symbols.push_back(symbol);
    }
}

std::vector<Symbol> SymbolManager::findSymbols(const std::string& name) {
    std::vector<Symbol> results;
    for (const auto& symbol : symbols) {
        if (symbol.name == name) {
            results.push_back(symbol);
        }
    }
    return results;
}

std::vector<Symbol> SymbolManager::getSymbolsInFile(const std::string& filePath) {
    std::vector<Symbol> results;
    for (const auto& symbol : symbols) {
        if (symbol.file_path == filePath) {
            results.push_back(symbol);
        }
    }
    return results;
}

std::vector<Symbol> SymbolManager::getAllSymbols() const {
    return symbols;
}

void SymbolManager::clear() {
    symbols.clear();
}

} // namespace devpilot
