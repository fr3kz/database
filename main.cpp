#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

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

    void saveToFile(const std::string& filename) {
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
                        file << fmt::format("{}: {}, ", column.name, row.at(column.name));
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

public:
    void createTable(const std::string& tableName, const std::vector<Column>& columns) {
        Table table;
        table.name = tableName;
        table.columns = columns;
        tables[tableName] = table;
        fmt::print("Table {} created\n", tableName);
    }

    void addColumnToTable(const std::string& tableName, const Column& newColumn) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            auto columnIt = std::find_if(it->second.columns.begin(), it->second.columns.end(),
                                         [&newColumn](const Column& existingColumn) {
                                             return existingColumn.name == newColumn.name;
                                         });

            if (columnIt == it->second.columns.end()) {
                it->second.columns.push_back(newColumn);
                for (auto& row : it->second.rows) {
                    row[newColumn.name] = "";
                }
                fmt::print("Column {} added to table {}\n", newColumn.name, tableName);
            } else {
                fmt::print("Error: Column {} already exists in table {}\n", newColumn.name, tableName);
            }
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void insertData(const std::string& tableName, const std::map<std::string, std::string>& data) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            std::map<std::string, std::string> newRow;
            for (const auto& column : it->second.columns) {
                newRow[column.name] = "";
            }

            for (const auto& entry : data) {
                auto columnIt = newRow.find(entry.first);
                if (columnIt != newRow.end()) {
                    columnIt->second = entry.second;
                } else {
                    fmt::print("Error: Column {} not found in table {}\n", entry.first, tableName);
                    return;
                }
            }

            it->second.rows.push_back(newRow);
            fmt::print("Data inserted into table {}\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void saveToBackup(const std::string& filename) {
        saveToFile(filename);
    }
};

int main() {
    SimpleDatabase database;

    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "createTable") {
            std::string tableName;
            iss >> tableName;

            std::vector<Column> columns;
            std::string colName, colType;
            while (iss >> colName >> colType) {
                columns.push_back({colName, colType});
            }

            database.createTable(tableName, columns);
        } else if (cmd == "addColumn") {
            std::string tableName;
            iss >> tableName;

            std::string colName, colType;
            iss >> colName >> colType;

            database.addColumnToTable(tableName, {colName, colType});
        } else if (cmd == "insert") {
            std::string tableName;
            iss >> tableName;

            std::map<std::string, std::string> data;
            std::string colInfo;
            while (iss >> colInfo) {
                size_t colonPos = colInfo.find(':');
                if (colonPos != std::string::npos) {
                    std::string colName = colInfo.substr(0, colonPos);
                    std::string colValue = colInfo.substr(colonPos + 1);
                    data[colName] = colValue;
                } else {
                    fmt::print("Error: Invalid column format in command\n");
                    break;
                }
            }

            database.insertData(tableName, data);
        }

        else if (cmd == "save") {
            std::string filename;
            iss >> filename;
            database.saveToBackup(filename);
        } else if (cmd == "exit") {
            break;
        } else {
            std::cout << "Unknown command. Try again.\n";
        }
    }

    return 0;
}


/*

createTable Employees ID int Name string Salary double Department string
        insert Employees ID:1 Name:John Salary:50000 Department:HR
save a.txt

exit

*/