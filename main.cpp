#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>  // Dodane dla std::remove

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

    void updateData(const std::string& tableName, const std::map<std::string, std::string>& setValues, const std::string& conditionColumn, const std::string& conditionValue) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            for (auto& row : it->second.rows) {
                // Sprawdź warunek
                auto conditionIt = row.find(conditionColumn);
                if (conditionIt != row.end() && conditionIt->second == conditionValue) {
                    // Aktualizuj wartości
                    for (const auto& entry : setValues) {
                        auto columnIt = row.find(entry.first);
                        if (columnIt != row.end()) {
                            columnIt->second = entry.second;
                        } else {
                            fmt::print("Error: Column {} not found in table {}\n", entry.first, tableName);
                            return;
                        }
                    }
                    fmt::print("Data updated in table {}\n", tableName);
                    return;
                }
            }
            fmt::print("Error: No rows found in table {} with condition\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void deleteData(const std::string& tableName, const std::string& conditionColumn, const std::string& conditionValue) {
        auto it = tables.find(tableName);
        if (it != tables.end()) {
            it->second.rows.erase(
                    std::remove_if(
                            it->second.rows.begin(),
                            it->second.rows.end(),
                            [&](const std::map<std::string, std::string>& row) {
                                auto conditionIt = row.find(conditionColumn);
                                return conditionIt != row.end() && conditionIt->second == conditionValue;
                            }),
                    it->second.rows.end());

            fmt::print("Data deleted from table {}\n", tableName);
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
        } else if (cmd == "update") {
            std::string tableName;
            iss >> tableName;

            std::map<std::string, std::string> setValues;
            std::string colInfo;
            while (iss >> colInfo) {
                size_t equalPos = colInfo.find('=');
                if (equalPos != std::string::npos) {
                    std::string colName = colInfo.substr(0, equalPos);
                    std::string colValue = colInfo.substr(equalPos + 1);
                    setValues[colName] = colValue;
                } else {
                    fmt::print("Error: Invalid set values format in command\n");
                    break;
                }
            }

            std::string conditionColumn, conditionValue;
            iss >> conditionColumn >> conditionValue;

            database.updateData(tableName, setValues, conditionColumn, conditionValue);
        } else if (cmd == "delete") {
            std::string tableName;
            iss >> tableName;

            std::string conditionColumn, conditionValue;
            iss >> conditionColumn >> conditionValue;

            database.deleteData(tableName, conditionColumn, conditionValue);
        } else if (cmd == "save") {
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
 delete Employees where Department=IT

exit

*/