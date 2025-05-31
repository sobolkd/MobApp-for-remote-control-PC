#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    bool init_database();
    bool insert_user(const char* username, const char* password);
    void print_all_users();

#ifdef __cplusplus
}
#endif
