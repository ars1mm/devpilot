#include "storage.hpp"
#include <iostream>
#include <sqlite3.h>

namespace devpilot {

// Single responsibility: Only handle SQLite database operations

SqliteStorage::SqliteStorage() 
    : db(nullptr), initialized(false), insertSymbolStmt(nullptr),
      searchSymbolStmt(nullptr), getSymbolsInFileStmt(nullptr),
      insertCallStmt(nullptr), getUsagesStmt(nullptr) {
}

SqliteStorage::~SqliteStorage() {
    close();
}

bool SqliteStorage::initialize(const std::string& dbPath) {
    if (initialized) {
        return true;
    }
    
    int result = sqlite3_open(dbPath.c_str(), &db);
    if (result != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    createTables();
    prepareStatements();
    initialized = true;
    
    std::cout << "Database initialized: " << dbPath << std::endl;
    return true;
}

void SqliteStorage::close() {
    if (!initialized) {
        return;
    }
    
    cleanupStatements();
    
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    
    initialized = false;
}

bool SqliteStorage::isInitialized() const {
    return initialized;
}

void SqliteStorage::createTables() {
    const char* createSymbolsTable = R"(
        CREATE TABLE IF NOT EXISTS symbols (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            type TEXT NOT NULL,
            file_path TEXT NOT NULL,
            line_number INTEGER NOT NULL,
            column_number INTEGER NOT NULL,
            signature TEXT,
            parent_scope TEXT
        );
        CREATE INDEX IF NOT EXISTS idx_symbol_name ON symbols(name);
        CREATE INDEX IF NOT EXISTS idx_symbol_file ON symbols(file_path);
    )";
    
    const char* createCallsTable = R"(
        CREATE TABLE IF NOT EXISTS call_relationships (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            caller_name TEXT NOT NULL,
            callee_name TEXT NOT NULL,
            call_file TEXT NOT NULL,
            call_line INTEGER NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_caller ON call_relationships(caller_name);
        CREATE INDEX IF NOT EXISTS idx_callee ON call_relationships(callee_name);
    )";
    
    char* errMsg = nullptr;
    int result = sqlite3_exec(db, createSymbolsTable, nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        std::cerr << "Error creating symbols table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return;
    }
    
    result = sqlite3_exec(db, createCallsTable, nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        std::cerr << "Error creating calls table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return;
    }
}

void SqliteStorage::prepareStatements() {
    insertSymbolStmt = prepareStatement(
        "INSERT INTO symbols (name, type, file_path, line_number, column_number, signature, parent_scope) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    
    searchSymbolStmt = prepareStatement(
        "SELECT name, type, file_path, line_number, column_number, signature, parent_scope "
        "FROM symbols WHERE name LIKE ? ORDER BY name"
    );
    
    getSymbolsInFileStmt = prepareStatement(
        "SELECT name, type, file_path, line_number, column_number, signature, parent_scope "
        "FROM symbols WHERE file_path = ? ORDER BY line_number"
    );
    
    insertCallStmt = prepareStatement(
        "INSERT INTO call_relationships (caller_name, callee_name, call_file, call_line) "
        "VALUES (?, ?, ?, ?)"
    );
    
    getUsagesStmt = prepareStatement(
        "SELECT DISTINCT caller_name, call_file, call_line FROM call_relationships "
        "WHERE callee_name = ? ORDER BY caller_name"
    );
}

void SqliteStorage::cleanupStatements() {
    if (insertSymbolStmt) { sqlite3_finalize(insertSymbolStmt); insertSymbolStmt = nullptr; }
    if (searchSymbolStmt) { sqlite3_finalize(searchSymbolStmt); searchSymbolStmt = nullptr; }
    if (getSymbolsInFileStmt) { sqlite3_finalize(getSymbolsInFileStmt); getSymbolsInFileStmt = nullptr; }
    if (insertCallStmt) { sqlite3_finalize(insertCallStmt); insertCallStmt = nullptr; }
    if (getUsagesStmt) { sqlite3_finalize(getUsagesStmt); getUsagesStmt = nullptr; }
}

sqlite3_stmt* SqliteStorage::prepareStatement(const std::string& sql) {
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return stmt;
}

bool SqliteStorage::storeSymbol(const Symbol& symbol) {
    if (!initialized || !insertSymbolStmt) {
        return false;
    }
    
    sqlite3_reset(insertSymbolStmt);
    
    sqlite3_bind_text(insertSymbolStmt, 1, symbol.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertSymbolStmt, 2, symbolTypeToString(symbol.type).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertSymbolStmt, 3, symbol.file_path.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(insertSymbolStmt, 4, symbol.line_number);
    sqlite3_bind_int(insertSymbolStmt, 5, symbol.column_number);
    sqlite3_bind_text(insertSymbolStmt, 6, symbol.signature.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertSymbolStmt, 7, symbol.parent_scope.c_str(), -1, SQLITE_STATIC);
    
    int result = sqlite3_step(insertSymbolStmt);
    return result == SQLITE_DONE;
}

std::vector<Symbol> SqliteStorage::searchSymbols(const std::string& query) {
    std::vector<Symbol> results;
    
    if (!initialized || !searchSymbolStmt) {
        return results;
    }
    
    sqlite3_reset(searchSymbolStmt);
    std::string searchPattern = "%" + query + "%";
    sqlite3_bind_text(searchSymbolStmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(searchSymbolStmt) == SQLITE_ROW) {
        results.push_back(createSymbolFromRow(searchSymbolStmt));
    }
    
    return results;
}

std::vector<Symbol> SqliteStorage::getSymbolsInFile(const std::string& filePath) {
    std::vector<Symbol> results;
    
    if (!initialized || !getSymbolsInFileStmt) {
        return results;
    }
    
    sqlite3_reset(getSymbolsInFileStmt);
    sqlite3_bind_text(getSymbolsInFileStmt, 1, filePath.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(getSymbolsInFileStmt) == SQLITE_ROW) {
        results.push_back(createSymbolFromRow(getSymbolsInFileStmt));
    }
    
    return results;
}

std::vector<Symbol> SqliteStorage::getAllSymbols() {
    std::vector<Symbol> results;
    
    if (!initialized) {
        return results;
    }
    
    const char* sql = "SELECT name, type, file_path, line_number, column_number, signature, parent_scope FROM symbols ORDER BY name";
    sqlite3_stmt* stmt = prepareStatement(sql);
    
    if (!stmt) {
        return results;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(createSymbolFromRow(stmt));
    }
    
    sqlite3_finalize(stmt);
    return results;
}

bool SqliteStorage::storeCallRelationship(const std::string& caller, const std::string& callee,
                                         const std::string& file, int line) {
    if (!initialized || !insertCallStmt) {
        return false;
    }
    
    sqlite3_reset(insertCallStmt);
    sqlite3_bind_text(insertCallStmt, 1, caller.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertCallStmt, 2, callee.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertCallStmt, 3, file.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(insertCallStmt, 4, line);
    
    int result = sqlite3_step(insertCallStmt);
    return result == SQLITE_DONE;
}

std::vector<std::string> SqliteStorage::getSymbolUsages(const std::string& symbolName) {
    std::vector<std::string> results;
    
    if (!initialized || !getUsagesStmt) {
        return results;
    }
    
    sqlite3_reset(getUsagesStmt);
    sqlite3_bind_text(getUsagesStmt, 1, symbolName.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(getUsagesStmt) == SQLITE_ROW) {
        const char* caller = (const char*)sqlite3_column_text(getUsagesStmt, 0);
        const char* file = (const char*)sqlite3_column_text(getUsagesStmt, 1);
        int line = sqlite3_column_int(getUsagesStmt, 2);
        
        std::string usage = std::string(caller) + " (" + file + ":" + std::to_string(line) + ")";
        results.push_back(usage);
    }
    
    return results;
}

std::vector<std::string> SqliteStorage::getSymbolCallees(const std::string& symbolName) {
    std::vector<std::string> results;
    
    if (!initialized) {
        return results;
    }
    
    const char* sql = "SELECT DISTINCT callee_name FROM call_relationships WHERE caller_name = ? ORDER BY callee_name";
    sqlite3_stmt* stmt = prepareStatement(sql);
    
    if (!stmt) {
        return results;
    }
    
    sqlite3_bind_text(stmt, 1, symbolName.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* callee = (const char*)sqlite3_column_text(stmt, 0);
        results.push_back(std::string(callee));
    }
    
    sqlite3_finalize(stmt);
    return results;
}

bool SqliteStorage::clearDatabase() {
    if (!initialized) {
        return false;
    }
    
    const char* clearSql = "DELETE FROM symbols; DELETE FROM call_relationships;";
    char* errMsg = nullptr;
    int result = sqlite3_exec(db, clearSql, nullptr, nullptr, &errMsg);
    
    if (result != SQLITE_OK) {
        std::cerr << "Error clearing database: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

Symbol SqliteStorage::createSymbolFromRow(sqlite3_stmt* stmt) {
    Symbol symbol;
    symbol.name = (const char*)sqlite3_column_text(stmt, 0);
    symbol.type = stringToSymbolType((const char*)sqlite3_column_text(stmt, 1));
    symbol.file_path = (const char*)sqlite3_column_text(stmt, 2);
    symbol.line_number = sqlite3_column_int(stmt, 3);
    symbol.column_number = sqlite3_column_int(stmt, 4);
    
    const char* signature = (const char*)sqlite3_column_text(stmt, 5);
    symbol.signature = signature ? std::string(signature) : "";
    
    const char* parent_scope = (const char*)sqlite3_column_text(stmt, 6);
    symbol.parent_scope = parent_scope ? std::string(parent_scope) : "";
    
    return symbol;
}

void SqliteStorage::logError(const std::string& operation) {
    if (db) {
        std::cerr << "SQLite error in " << operation << ": " << sqlite3_errmsg(db) << std::endl;
    }
}

} // namespace devpilot
