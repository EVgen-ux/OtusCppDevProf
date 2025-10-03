
#include "database.hpp"
#include <stdexcept>

Database::Database() {
    if (sqlite3_open(":memory:", &db_) != SQLITE_OK) {
        throw std::runtime_error("Cannot open database");
    }
    
    if (sqlite3_exec(db_, "CREATE TABLE A(id INT PRIMARY KEY, name TEXT);", 0, 0, 0) != SQLITE_OK ||
        sqlite3_exec(db_, "CREATE TABLE B(id INT PRIMARY KEY, name TEXT);", 0, 0, 0) != SQLITE_OK) {
        throw std::runtime_error("Cannot create tables");
    }
}

Database::~Database() {
    sqlite3_close(db_);
}

std::string Database::insert(const std::string& table, int id, const std::string& name) {
    std::string check_sql = "SELECT id FROM " + table + " WHERE id = " + std::to_string(id) + ";";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, check_sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return "ERR: Duplicate ID " + std::to_string(id) + " in table " + table;
        }
        sqlite3_finalize(stmt);
    }
    std::string sql = "INSERT INTO " + table + " VALUES(" + std::to_string(id) + ",'" + name + "');";
    return sqlite3_exec(db_, sql.c_str(), 0, 0, 0) == SQLITE_OK ? "OK" : "ERR: Database error";
}

std::string Database::truncate(const std::string& table) {
    std::string sql = "DELETE FROM " + table + ";";
    return sqlite3_exec(db_, sql.c_str(), 0, 0, 0) == SQLITE_OK ? "OK" : "ERR";
}

std::string Database::intersection() {
    std::string result;
    sqlite3_stmt* stmt;
    
    const char* sql = "SELECT a.id, a.name, b.name FROM A a JOIN B b ON a.id = b.id ORDER BY a.id;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, 0) != SQLITE_OK) {
        return "ERR";
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result += std::to_string(sqlite3_column_int(stmt, 0)) + "," +
                 reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) + "," +
                 reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) + "\n";
    }
    
    sqlite3_finalize(stmt);
    return result.empty() ? "OK" : result + "OK";
}

std::string Database::symmetric_difference() {
    std::string result;
    sqlite3_stmt* stmt;
    
    const char* sql = 
        "SELECT id, name, '' FROM A WHERE id NOT IN (SELECT id FROM B) "
        "UNION ALL "
        "SELECT id, '', name FROM B WHERE id NOT IN (SELECT id FROM A) "
        "ORDER BY id;";
        
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, 0) != SQLITE_OK) {
        return "ERR";
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result += std::to_string(sqlite3_column_int(stmt, 0)) + "," +
                 reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) + "," +
                 reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) + "\n";
    }
    
    sqlite3_finalize(stmt);
    return result.empty() ? "OK" : result + "OK";
}