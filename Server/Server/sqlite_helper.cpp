#include "sqlite_helper.h"
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

#include <sqlite3.h>
#include <string>
#include <sstream>
#include <iostream>

// hashing password
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

// create Database
bool init_database() {
    sqlite3* db;
    if (sqlite3_open("users.db", &db) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password_hash TEXT);";
    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        sqlite3_free(err);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

// insert user
bool insert_user(const char* username, const char* password) {
    sqlite3* db;
    if (sqlite3_open("users.db", &db) != SQLITE_OK) {
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

// print users with password-hash
void print_all_users() {
    sqlite3* db;
    if (sqlite3_open("users.db", &db) != SQLITE_OK) {
        std::cerr << "Cannot open DB.\n";
        return;
    }

    const char* sql = "SELECT id, username, password_hash FROM users;";
    char* err = nullptr;
    sqlite3_exec(db, sql, callback, nullptr, &err);
    if (err) sqlite3_free(err);
    sqlite3_close(db);
}
