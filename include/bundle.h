#pragma once
#include <stdio.h>

int bundle_copy(const char *src_app, const char *dest_app);
int bundle_find_executable(const char *app_path, char *out, size_t out_len);
int bundle_rename_executable(const char *app_path, const char *old_name, const char *new_name);
int bundle_resign(const char *exec_path);
