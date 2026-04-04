#include "bundle.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>

int bundle_copy(const char *src_app, const char *dest_app) {
    char cmd[8192];
    snprintf(cmd, sizeof(cmd), "cp -r \"%s\" \"%s\"", src_app, dest_app);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: failed to copy '%s' to '%s'\n", src_app, dest_app);
        return -1;
    }
    return 0;
}

int bundle_find_executable(const char *app_path, char *out, size_t out_len) {
    char plist[4096];
    snprintf(plist, sizeof(plist), "%s/Contents/Info.plist", app_path);

    char cmd[8192];
    snprintf(cmd, sizeof(cmd),
        "defaults read \"%s\" CFBundleExecutable 2>/dev/null", plist);

    FILE *f = popen(cmd, "r");
    if (f) {
        char exec_name[256] = {0};
        if (fgets(exec_name, sizeof(exec_name), f)) {
            exec_name[strcspn(exec_name, "\n")] = '\0';
            pclose(f);
            if (strlen(exec_name) > 0) {
                strncpy(out, exec_name, out_len - 1);
                out[out_len - 1] = '\0';
                return 0;
            }
        } else {
            pclose(f);
        }
    }

    // Fall back to bundle name (strip path and .app suffix)
    char app_copy[4096];
    strncpy(app_copy, app_path, sizeof(app_copy) - 1);
    size_t len = strlen(app_copy);
    if (app_copy[len - 1] == '/') app_copy[len - 1] = '\0';

    char *base = basename(app_copy);
    char *dot = strstr(base, ".app");
    if (dot) *dot = '\0';

    strncpy(out, base, out_len - 1);
    out[out_len - 1] = '\0';
    return 0;
}

int bundle_rename_executable(const char *app_path, const char *old_name, const char *new_name) {
    char old_path[4096], new_path[4096];
    snprintf(old_path, sizeof(old_path), "%s/Contents/MacOS/%s", app_path, old_name);
    snprintf(new_path, sizeof(new_path), "%s/Contents/MacOS/%s", app_path, new_name);

    if (rename(old_path, new_path) != 0) {
        perror("rename executable");
        return -1;
    }

    chmod(new_path, 0755);
    return 0;
}

int bundle_resign(const char *exec_path) {
    char cmd[8192];

    snprintf(cmd, sizeof(cmd), "codesign --remove-signature \"%s\"", exec_path);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: failed to remove signature from '%s'. Please ensure you have Xcode Command Line Tools installed.\n", exec_path);
        return -1;
    }

    snprintf(cmd, sizeof(cmd), "codesign -s - \"%s\"", exec_path);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: failed to ad-hoc sign '%s'\n", exec_path);
        return -1;
    }

    return 0;
}