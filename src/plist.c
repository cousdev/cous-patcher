#include "plist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLISTBUDDY "/usr/libexec/PlistBuddy"

static int plistbuddy_set(const char *plist_path, const char *key, const char *value) {
    char cmd[8192];
    snprintf(cmd, sizeof(cmd),
        "%s -c \"Set :%s %s\" \"%s\"", PLISTBUDDY, key, value, plist_path);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: PlistBuddy failed to set %s\n", key);
        return -1;
    }
    return 0;
}

int plist_patch(const char *dest_app_path, const char *fake_name) {
    char plist_path[4096];
    snprintf(plist_path, sizeof(plist_path), "%s/Contents/Info.plist", dest_app_path);

    if (plistbuddy_set(plist_path, "CFBundleName",        fake_name) != 0) return -1;
    if (plistbuddy_set(plist_path, "CFBundleExecutable",  fake_name) != 0) return -1;

    return 0;
}