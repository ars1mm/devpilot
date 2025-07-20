#include <iostream>
#include <cassert>
#include <sqlite3.h>

// Basic test functions
void test_sqlite_connection() {
    sqlite3* db;
    int rc = sqlite3_open(":memory:", &db);
    assert(rc == SQLITE_OK);
    
    if (db) {
        sqlite3_close(db);
    }
    
    std::cout << "✓ SQLite connection test passed\n";
}

void test_basic_functionality() {
    // Test that we can create a simple in-memory database
    sqlite3* db;
    char* errMsg = nullptr;
    
    int rc = sqlite3_open(":memory:", &db);
    assert(rc == SQLITE_OK);
    
    // Create a simple test table
    const char* sql = "CREATE TABLE test(id INTEGER PRIMARY KEY, name TEXT);";
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    assert(rc == SQLITE_OK);
    
    // Insert test data
    sql = "INSERT INTO test(name) VALUES('test_entry');";
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    assert(rc == SQLITE_OK);
    
    sqlite3_close(db);
    std::cout << "✓ Basic SQLite operations test passed\n";
}

#ifdef HAVE_TREE_SITTER
#include <tree_sitter/api.h>

void test_tree_sitter_availability() {
    // Just test that we can create a parser
    TSParser* parser = ts_parser_new();
    assert(parser != nullptr);
    ts_parser_delete(parser);
    std::cout << "✓ TreeSitter availability test passed\n";
}
#endif

int main() {
    std::cout << "Running DevPilot basic tests...\n\n";
    
    try {
        test_sqlite_connection();
        test_basic_functionality();
        
#ifdef HAVE_TREE_SITTER
        test_tree_sitter_availability();
        std::cout << "✓ TreeSitter support enabled\n";
#else
        std::cout << "! TreeSitter support disabled (stub parser mode)\n";
#endif
        
        std::cout << "\n✅ All basic tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
