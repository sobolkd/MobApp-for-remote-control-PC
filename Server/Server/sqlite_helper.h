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

#ifdef __cplusplus
#include <string>
bool check_user_exists(const std::string& username, const std::string& password);
#endif
