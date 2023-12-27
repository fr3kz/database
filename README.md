# Simple Database Management System

This project implements a simple database management system in C++. It provides functionalities to create tables, add columns, insert data, update data, delete data, and perform queries on tables. The data is stored in-memory, and the system allows for saving the database to a file.

## Usage

### Prerequisites

Make sure you have the following prerequisites installed:

- C++ compiler
- fmt library (`fmt/core.h` is used for formatting)

### Compilation

Compile the project using the following command:

```bash
g++ -std=c++14 -o database main.cpp -lfmt
```
### Example Commands
- createTable Employees ID int Name string Salary double Department string
- addColumn Employees PhoneNumber int
- insert Employees ID:2 Name:John Salary:50000 Department:HR
- update Employees Name:Artur ID:4 where ID:2
- query Employees where Name:John
- query Employees Name: Salary: where ID:2
- delete Employees ID:1
- save backup.txt
- exit

###Przyklad

![Alt Text](https://github.com/fr3kz/database/blob/main/Zrzut%20ekranu%202023-12-27%20o%2018.30.49.png)

