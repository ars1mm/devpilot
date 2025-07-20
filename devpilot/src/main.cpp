#include "parser.hpp"
#include "storage.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace devpilot {

// Single responsibility: Only handle CLI commands and user interaction

class DevPilotCLI {
public:
    int run(int argc, char* argv[]);
    
private:
    CppParser parser;
    SqliteStorage storage;
    
    // Command implementations
    int indexCommand(const std::string& projectPath);
    int searchCommand(const std::string& query);
    int usagesCommand(const std::string& symbolName);
    int helpCommand();
    
    // Helper methods
    void printUsage();
    void printSymbol(const Symbol& symbol);
    void printSymbols(const std::vector<Symbol>& symbols);
    std::vector<std::string> findCppFiles(const std::string& directory);
    bool isCppFile(const std::string& filename);
};

int DevPilotCLI::run(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::string command = argv[1];
    
    // Initialize storage with default database
    if (!storage.initialize("devpilot.db")) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    
    if (command == "index") {
        if (argc < 3) {
            std::cerr << "Usage: devpilot index <project_path>" << std::endl;
            return 1;
        }
        return indexCommand(argv[2]);
    }
    else if (command == "search") {
        if (argc < 3) {
            std::cerr << "Usage: devpilot search <symbol_name>" << std::endl;
            return 1;
        }
        return searchCommand(argv[2]);
    }
    else if (command == "usages") {
        if (argc < 3) {
            std::cerr << "Usage: devpilot usages <symbol_name>" << std::endl;
            return 1;
        }
        return usagesCommand(argv[2]);
    }
    else if (command == "help" || command == "--help" || command == "-h") {
        return helpCommand();
    }
    else {
        std::cerr << "Unknown command: " << command << std::endl;
        printUsage();
        return 1;
    }
}

int DevPilotCLI::indexCommand(const std::string& projectPath) {
    std::cout << "Indexing C++ project: " << projectPath << std::endl;
    
    if (!std::filesystem::exists(projectPath)) {
        std::cerr << "Error: Path does not exist: " << projectPath << std::endl;
        return 1;
    }
    
    if (!parser.isInitialized()) {
        std::cerr << "Error: Parser not initialized" << std::endl;
        return 1;
    }
    
    // Find all C++ files
    auto cppFiles = findCppFiles(projectPath);
    std::cout << "Found " << cppFiles.size() << " C++ files" << std::endl;
    
    // Clear existing data for this project
    storage.clearDatabase();
    
    int fileCount = 0;
    int symbolCount = 0;
    
    for (const auto& file : cppFiles) {
        std::cout << "Parsing: " << file << std::endl;
        
        auto symbols = parser.parseFile(file);
        for (const auto& symbol : symbols) {
            if (storage.storeSymbol(symbol)) {
                symbolCount++;
            }
        }
        fileCount++;
    }
    
    std::cout << "Indexing complete!" << std::endl;
    std::cout << "Files processed: " << fileCount << std::endl;
    std::cout << "Symbols extracted: " << symbolCount << std::endl;
    
    return 0;
}

int DevPilotCLI::searchCommand(const std::string& query) {
    std::cout << "Searching for: " << query << std::endl;
    
    auto symbols = storage.searchSymbols(query);
    
    if (symbols.empty()) {
        std::cout << "No symbols found matching: " << query << std::endl;
        return 0;
    }
    
    std::cout << "Found " << symbols.size() << " symbol(s):" << std::endl;
    printSymbols(symbols);
    
    return 0;
}

int DevPilotCLI::usagesCommand(const std::string& symbolName) {
    std::cout << "Finding usages of: " << symbolName << std::endl;
    
    auto usages = storage.getSymbolUsages(symbolName);
    
    if (usages.empty()) {
        std::cout << "No usages found for: " << symbolName << std::endl;
        return 0;
    }
    
    std::cout << "Found " << usages.size() << " usage(s):" << std::endl;
    for (const auto& usage : usages) {
        std::cout << "  " << usage << std::endl;
    }
    
    return 0;
}

int DevPilotCLI::helpCommand() {
    std::cout << "DevPilot - Local AI Codebase Navigator" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  devpilot <command> [arguments]" << std::endl;
    std::cout << std::endl;
    std::cout << "COMMANDS:" << std::endl;
    std::cout << "  index <path>     Index a C++ project directory" << std::endl;
    std::cout << "  search <name>    Search for symbols by name" << std::endl;
    std::cout << "  usages <name>    Find where a symbol is used" << std::endl;
    std::cout << "  help             Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  devpilot index /path/to/cpp/project" << std::endl;
    std::cout << "  devpilot search \"calculateSum\"" << std::endl;
    std::cout << "  devpilot usages \"processData\"" << std::endl;
    std::cout << std::endl;
    
    return 0;
}

void DevPilotCLI::printUsage() {
    std::cout << "Usage: devpilot <command> [arguments]" << std::endl;
    std::cout << "Run 'devpilot help' for more information." << std::endl;
}

void DevPilotCLI::printSymbol(const Symbol& symbol) {
    std::cout << "  " << symbolTypeToString(symbol.type) << " " << symbol.name;
    std::cout << " (" << symbol.file_path << ":" << symbol.line_number << ")";
    if (!symbol.signature.empty() && symbol.signature != symbol.name) {
        std::cout << " - " << symbol.signature;
    }
    std::cout << std::endl;
}

void DevPilotCLI::printSymbols(const std::vector<Symbol>& symbols) {
    for (const auto& symbol : symbols) {
        printSymbol(symbol);
    }
}

std::vector<std::string> DevPilotCLI::findCppFiles(const std::string& directory) {
    std::vector<std::string> cppFiles;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && isCppFile(entry.path().filename().string())) {
                cppFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error traversing directory: " << e.what() << std::endl;
    }
    
    std::sort(cppFiles.begin(), cppFiles.end());
    return cppFiles;
}

bool DevPilotCLI::isCppFile(const std::string& filename) {
    // Check for C++ file extensions
    std::vector<std::string> extensions = {".cpp", ".cxx", ".cc", ".hpp", ".h", ".hxx"};
    
    for (const auto& ext : extensions) {
        if (filename.length() >= ext.length() &&
            filename.substr(filename.length() - ext.length()) == ext) {
            return true;
        }
    }
    
    return false;
}

} // namespace devpilot

// Main entry point
int main(int argc, char* argv[]) {
    devpilot::DevPilotCLI cli;
    return cli.run(argc, argv);
}
