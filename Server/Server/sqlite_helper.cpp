#include "sqlite_helper.h"
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

#include <sqlite3.h>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>

const char* DB_PATH = "users.db";

void SetWorkingDirectoryToExePath() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::filesystem::path path(exePath);
    std::filesystem::current_path(path.parent_path());
}

// Hashing password
static std::string hash_password(const std::string& password) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    PUCHAR hashObject = NULL;
    DWORD hashObjectSize = 0, dataSize = 0;
    UCHAR hash[32];

    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) return "";
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjectSize, sizeof(DWORD), &dataSize, 0))) return "";

    hashObject = (PUCHAR)HeapAlloc(GetProcessHeap(), 0, hashObjectSize);
    if (!hashObject) return "";

    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, hashObject, hashObjectSize, NULL, 0, 0))) return "";
    if (!BCRYPT_SUCCESS(BCryptHashData(hHash, (PUCHAR)password.c_str(), password.size(), 0))) return "";
    if (!BCRYPT_SUCCESS(BCryptFinishHash(hHash, hash, sizeof(hash), 0))) return "";

    std::ostringstream oss;
    for (int i = 0; i < 32; ++i)
        oss << std::hex << (hash[i] >> 4) << (hash[i] & 0xF);

    if (hHash) BCryptDestroyHash(hHash);
    if (hashObject) HeapFree(GetProcessHeap(), 0, hashObject);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);

    return oss.str();
}

// Init database
bool init_database() {
    SetWorkingDirectoryToExePath();

    sqlite3* db;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        std::string msg = "Failed to open DB: ";
        msg += sqlite3_errmsg(db);
        MessageBoxA(NULL, msg.c_str(), "SQLite Error", MB_ICONERROR);
        sqlite3_close(db);
        return false;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password_hash TEXT);";
    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = "Failed to create table: ";
        msg += err ? err : "Unknown error";
        MessageBoxA(NULL, msg.c_str(), "SQLite Error", MB_ICONERROR);
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

// Add user
bool insert_user(const char* username, const char* password) {
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    std::string hash = hash_password(password);
    std::string sql = "INSERT INTO users(username, password_hash) VALUES('" + std::string(username) + "', '" + hash + "');";

    char* err = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }
    std::cout << "Added user: " + std::string(username) << std::endl;
    sqlite3_close(db);
    return true;
}

static int callback(void*, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
    }
    std::cout << std::endl;
    return 0;
}

// print users
void print_all_users() {
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        std::cerr << "Cannot open DB.\n";
        return;
    }

    const char* sql = "SELECT id, username, password_hash FROM users;";
    char* err = nullptr;
    sqlite3_exec(db, sql, callback, nullptr, &err);
    if (err) sqlite3_free(err);
    sqlite3_close(db);
}

// check user
bool check_user_exists(const std::string& username, const std::string& password) {
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        return false;
    }

    std::string passwordHash = hash_password(password);
    const char* sql = "SELECT COUNT(*) FROM users WHERE username = ? AND password_hash = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_STATIC);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        exists = (count > 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return exists;
}

// delete all users
bool delete_all_users() {
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        return false;
    }

    const char* sql = "DELETE FROM users;";
    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Error deleting users: " << err << "\n";
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

// delete user
std::string delete_user_by_id(int userId) {
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        return "Failed to open database.";
    }

    const char* sql = "DELETE FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return "Failed to prepare delete statement.";
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "Failed to bind user ID.";
    }

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "Failed to execute delete.";
    }

    int changes = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (changes > 0) {
        return "User successfully deleted.";
    }
    else {
        return "No user with such ID found.";
    }
}
