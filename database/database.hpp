
#pragma once

#include <sqlite3.h>
#include <string>

class Database {
public:
    Database();
    ~Database();
    
    std::string insert(const std::string& table, int id, const std::string& name);
    std::string truncate(const std::string& table);
    std::string intersection();
    std::string symmetric_difference();

private:
    sqlite3* db_;
};