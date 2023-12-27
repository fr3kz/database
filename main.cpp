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

class SimpleDatabase;

struct Table {
    friend class SimpleDatabase;

private:
    std::string name;
    std::vector<Column> columns;
    std::vector<std::map<std::string, std::string>> rows;

    std::string getColumnType(const std::string& columnName) const {
        for (const auto& column : columns) {
            if (column.name == columnName) {
                return column.type;
            }
        }
        return "";
    }

public:
    Table() {}

    Table(const std::string& tableName, const std::vector<Column>& tableColumns) : name(tableName), columns(tableColumns) {}

    void createRow(const std::map<std::string, std::string>& data) {
        std::map<std::string, std::string> newRow;
        for (const auto& column : columns) {
            newRow[column.name] = "";
        }

        for (const auto& entry : data) {
            auto columnIt = newRow.find(entry.first);
            if (columnIt != newRow.end()) {
                columnIt->second = entry.second;
            } else {
                fmt::print("Error: Column {} not found in table {}\n", entry.first, name);
                return;
            }
        }

        rows.push_back(newRow);
    }
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
        Table table(tableName, columns);
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
            it->second.createRow(data);
            fmt::print("Data inserted into table {}\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void updateData(const std::string& tableName, const std::map<std::string, std::string>& updateData, const std::map<std::string, std::string>& whereClause) {
        auto it = tables.find(tableName);

        if (it != tables.end()) {
            for (auto& row : it->second.rows) {
                bool whereConditionSatisfied = true;

                for (const auto& whereEntry : whereClause) {
                    auto columnIt = row.find(whereEntry.first);
                    if (columnIt == row.end() || columnIt->second != whereEntry.second) {
                        whereConditionSatisfied = false;
                        break;
                    }
                }

                if (whereConditionSatisfied) {
                    for (const auto& entry : updateData) {
                        auto columnIt = row.find(entry.first);
                        if (columnIt != row.end()) {
                            if (it->second.getColumnType(entry.first) == "int") {
                                columnIt->second = std::to_string(std::stoi(entry.second));
                            } else if (it->second.getColumnType(entry.first) == "double") {
                                columnIt->second = std::to_string(std::stod(entry.second));
                            } else {
                                columnIt->second = entry.second;
                            }
                        } else {
                            fmt::print("Error: Column {} not found in table {}\n", entry.first, tableName);
                            return;
                        }
                    }
                }
            }

            fmt::print("Data updated in table {}\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void deleteData( std::string& tableName, std::map<std::string, std::string>& whereClause) {
        auto it = tables.find(tableName);

        if (it != tables.end()) {
            for (auto rowIt = it->second.rows.begin(); rowIt != it->second.rows.end();) {
                bool whereConditionSatisfied = true;

                for (const auto& whereEntry : whereClause) {
                    auto columnIt = rowIt->find(whereEntry.first);
                    if (columnIt == rowIt->end() || columnIt->second != whereEntry.second) {
                        whereConditionSatisfied = false;
                        break;
                    }
                }

                if (whereConditionSatisfied) {
                    rowIt = it->second.rows.erase(rowIt);
                } else {
                    ++rowIt;
                }
            }

            fmt::print("Data deleted from table {}\n", tableName);
        } else {
            fmt::print("Error: Table {} not found\n", tableName);
        }
    }

    void query(const std::string& tableName, const std::vector<std::string>& selectClause, const std::map<std::string, std::string>& whereClause) {
        auto it = tables.find(tableName);

        if (it != tables.end()) {
            if (selectClause.empty()) {
                for (const auto& column : it->second.columns) {
                    std::cout << column.name << "\t";
                }
            } else {
                for (const auto& col : selectClause) {
                    std::cout << col << "\t";
                }
            }
            std::cout << "\n";

            for (const auto& row : it->second.rows) {
                bool whereConditionSatisfied = true;

                for (const auto& whereEntry : whereClause) {
                    auto columnIt = row.find(whereEntry.first);
                    if (columnIt == row.end() || columnIt->second != whereEntry.second) {
                        whereConditionSatisfied = false;
                        break;
                    }
                }

                if (whereConditionSatisfied) {
                    if (selectClause.empty()) {
                        for (const auto& column : it->second.columns) {
                            auto columnIt = row.find(column.name);
                            if (columnIt != row.end()) {
                                std::cout << columnIt->second << "\t";
                            } else {
                                fmt::print("Error: Column {} not found in table {}\n", column.name, tableName);
                                return;
                            }
                        }
                    } else {
                        for (const auto& col : selectClause) {
                            auto columnIt = row.find(col);
                            if (columnIt != row.end()) {
                                std::cout << columnIt->second << "\t";
                            } else {
                                fmt::print("Error: Column {} not found in table {}\n", col, tableName);
                                return;
                            }
                        }
                    }
                    std::cout << "\n";
                }
            }

            fmt::print("Query executed for table {}\n", tableName);
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
        } else if (cmd == "update") {
            std::string tableName;
            if (!(iss >> tableName)) {
                fmt::print("Error: Missing table name for update command\n");
                continue;
            }

            std::map<std::string, std::string> updateData;
            std::map<std::string, std::string> whereClause;

            std::string colInfo;
            bool isWhereClause = false;

            while (iss >> colInfo) {
                if (colInfo == "where") {
                    isWhereClause = true;
                    continue;
                }

                size_t colonPos = colInfo.find(':');
                if (colonPos != std::string::npos) {
                    std::string colName = colInfo.substr(0, colonPos);
                    std::string colValue = colInfo.substr(colonPos + 1);

                    if (isWhereClause) {
                        whereClause[colName] = colValue;
                    } else {
                        updateData[colName] = colValue;
                    }
                } else {
                    fmt::print("Error: Invalid column format in command\n");
                    break;
                }
            }

            database.updateData(tableName, updateData, whereClause);
        }else if (cmd == "query") {
            std::string tablename;
            iss >> tablename;

            std::map<std::string, std::string> whereClause;
            std::vector<std::string> selectClause;

            std::string colInfo;
            bool isWhereClause = false;
            while (iss >> colInfo) {
                size_t colonPos = colInfo.find(':');

                if (colInfo == "where") {
                    isWhereClause = true;
                    continue;
                }

                if (colonPos != std::string::npos) {
                    std::string colName = colInfo.substr(0, colonPos);
                    std::string colValue = colInfo.substr(colonPos + 1);

                    if (isWhereClause) {
                        whereClause[colName] = colValue;
                    } else {
                        selectClause.push_back(colName);
                    }

                } else {
                    fmt::print("Error: Invalid column format in command\n");
                    break;
                }
            }


            database.query(tablename, selectClause, whereClause);
        }



        else if(cmd=="delete"){
            std::string tableName;
            iss >> tableName;

            std::map<std::string, std::string> whereClause;
            std::string colInfo;

            while(iss >> colInfo){
                size_t colonPos = colInfo.find(':');
                if (colonPos != std::string::npos) {
                    std::string colName = colInfo.substr(0, colonPos);
                    std::string colValue = colInfo.substr(colonPos + 1);
                    whereClause[colName] = colValue;
                } else {
                    fmt::print("Error: Invalid column format in command\n");
                    break;
                }
            }

            database.deleteData(tableName, whereClause);

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
        update Employees Name:Artur ID:4 where ID:2
        query Employees where Name:John
        query Employees Name: Salary: where ID:2
        delete Employees ID:1
        addColumn Employees Age int


        save a.txt

        exit

*/