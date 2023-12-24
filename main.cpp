#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include "fmt/core.h"

struct Column {
    std::string name;
    std::string type;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<std::map<std::string, std::string>> rows;
};

class SimpleDatabase {
private:
    std::map<std::string, Table> tables;

public:
    void createTable(const std::string& tableName, const std::vector<Column>& columns) {
        Table table;
        table.name = tableName;
        table.columns = columns;
        tables[tableName] = table;
        fmt::print("Table {} created\n", tableName);
    }

    void dropTable(const std::string& tableName) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            tables.erase(it);
            fmt::print("Table {} dropped\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void addColumn(const std::string& tableName, const Column& column) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            it->second.columns.push_back(column);
            fmt::print("Column {} added to table {}\n", column.name, tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void insertData(const std::string& tableName, const std::map<std::string, std::string>& data) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            it->second.rows.push_back(data);
            fmt::print("Data inserted into table {}\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void queryData(const std::string& tableName, const std::string& condition) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            fmt::print("Query results for table {} with condition '{}':\n", tableName, condition);
            for (const auto& row : it->second.rows) {
                bool meetsCondition = true;
                if (meetsCondition) {
                    for (const auto& entry : row) {
                        fmt::print("{}: {}, ", entry.first, entry.second);
                    }
                    fmt::print("\n");
                }
            }
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void saveToBackup(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (const auto& entry : tables) {
                file << fmt::format("Table: {}\n", entry.first);
                for (const auto& column : entry.second.columns) {
                    file << fmt::format("  {} ({})\n", column.name, column.type);
                }
                for (const auto& row : entry.second.rows) {
                    file << "  ";
                    for (const auto& column : entry.second.columns) {
                        auto valueIt = row.find(column.name);
                        if (valueIt != row.end()) {
                            file << fmt::format("{}: {}, ", column.name, valueIt->second);
                        } else {
                            file << fmt::format("{}: [Not Found], ", column.name);
                        }
                    }
                    file << "\n";
                }
            }
            file.close();
            fmt::print("Database saved to {}\n", filename);
        } else {
            fmt::print("Error: Unable to open file for saving\n");
        }
    }

    void loadFromBackup(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            Table currentTable;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string token;
                iss >> token;
                if (token == "Table:") {
                    if (!currentTable.name.empty()) {
                        tables[currentTable.name] = currentTable;
                    }
                    currentTable.name.clear();
                    currentTable.columns.clear();
                    currentTable.rows.clear();
                    iss >> currentTable.name;
                } else if (!currentTable.name.empty() && token.back() == ':') {
                    Column column;
                    column.name = token.substr(0, token.size() - 1);
                    iss >> token;
                    column.type = token;
                    currentTable.columns.push_back(column);
                } else if (!currentTable.name.empty() && token == "row") {
                    std::map<std::string, std::string> row;
                    while (iss >> token && token.back() == ',') {
                        token.pop_back();
                        std::string key = token.substr(0, token.find(':'));
                        std::string value = token.substr(token.find(':') + 1);
                        row[key] = value;
                    }
                    currentTable.rows.push_back(row);
                }
            }
            if (!currentTable.name.empty()) {
                tables[currentTable.name] = currentTable;
            }
            file.close();
            fmt::print("Database loaded from {}\n", filename);
        } else {
            fmt::print("Error: Unable to open file for loading\n");
        }
    }
};

int main() {
    SimpleDatabase database;

    std::string command;
    std::cout << "Enter commands (type 'exit' to quit):\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        std::istringstream iss(command);
        std::string operation;
        iss >> operation;

        if (operation == "createTable") {
            std::string tableName;
            iss >> tableName;

            std::vector<Column> columns;
            std::string columnName, columnType;
            while (iss >> columnName >> columnType) {
                columns.push_back({columnName, columnType});
            }

            database.createTable(tableName, columns);
        } else if (operation == "dropTable") {
            std::string tableName;
            iss >> tableName;
            database.dropTable(tableName);
        } else if (operation == "addColumn") {
            std::string tableName, columnName, columnType;
            iss >> tableName >> columnName >> columnType;
            database.addColumn(tableName, {columnName, columnType});
        } else if (operation == "insert") {
            std::string tableName, columnValuePair;
            iss >> tableName >> columnValuePair;

            std::map<std::string, std::string> data;
            std::istringstream dataStream(columnValuePair);
            std::string column, value;
            while (std::getline(dataStream, column, ':') && std::getline(dataStream, value, ' ')) {
                data[column] = value;
            }

            database.insertData(tableName, data);
        } else if (operation == "query") {
            std::string tableName, condition;
            iss >> tableName >> condition;
            database.queryData(tableName, condition);
        } else if (operation == "save") {
            std::string filename;
            iss >> filename;
            database.saveToBackup(filename);
        } else if (operation == "load") {
            std::string filename;
            iss >> filename;
            database.loadFromBackup(filename);
        } else {
            std::cout << "Unknown command. Try again.\n";
        }
    }

    return 0;
}
