#pragma once

#include "symbol.hpp"
#include <string>
#include <vector>
#include <memory>

// Forward declaration for SQLite
struct sqlite3;
struct sqlite3_stmt;

namespace devpilot {

class SqliteStorage {
public:
    SqliteStorage();
    ~SqliteStorage();
    
    // Single responsibility: Only handle SQLite database operations
    bool initialize(const std::string& dbPath);
    void close();
    
    // Symbol operations
    bool storeSymbol(const Symbol& symbol);
    std::vector<Symbol> searchSymbols(const std::string& query);
    std::vector<Symbol> getSymbolsInFile(const std::string& filePath);
    std::vector<Symbol> getAllSymbols();
    
    // Call relationship operations (for usage tracking)
    bool storeCallRelationship(const std::string& caller, const std::string& callee,
                              const std::string& file, int line);
    std::vector<std::string> getSymbolUsages(const std::string& symbolName);
    std::vector<std::string> getSymbolCallees(const std::string& symbolName);
    
    // Database management
    bool clearDatabase();
    bool isInitialized() const;
    
private:
    sqlite3* db;
    bool initialized;
    
    // Prepared statements for performance
    sqlite3_stmt* insertSymbolStmt;
    sqlite3_stmt* searchSymbolStmt;
    sqlite3_stmt* getSymbolsInFileStmt;
    sqlite3_stmt* insertCallStmt;
    sqlite3_stmt* getUsagesStmt;
    
    // Database setup
    void createTables();
    void prepareStatements();
    void cleanupStatements();
    
    // Helper methods
    sqlite3_stmt* prepareStatement(const std::string& sql);
    Symbol createSymbolFromRow(sqlite3_stmt* stmt);
    bool executeStatement(sqlite3_stmt* stmt);
    
    // Error handling
    void logError(const std::string& operation);
};

} // namespace devpilot
