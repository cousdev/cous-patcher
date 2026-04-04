#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "bundle.h"
#include "plist.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <target.app> <fake name>\n", argv[0]);
        fprintf(stderr, "  Example: %s /Applications/Steam.app 'not_steam'\n", argv[0]);
        return 1;
    }

    const char *target_app = argv[1];
    const char *fake_name  = argv[2];

    // Validate the target is a .app bundle
    struct stat st;
    if (stat(target_app, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: '%s' is not a valid .app bundle\n", target_app);
        return 1;
    }

    // Find the real executable name inside the bundle
    char real_exec_name[256];
    if (bundle_find_executable(target_app, real_exec_name, sizeof(real_exec_name)) != 0) {
        fprintf(stderr, "Error: could not locate executable inside '%s'\n", target_app);
        return 1;
    }

    // Build destination path in cwd
    char cwd[8192];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return 1;
    }

    char dest_app[8192];
    snprintf(dest_app, sizeof(dest_app), "%s/%s.app", cwd, fake_name);

    printf("Source      : %s\n", target_app);
    printf("Executable  : %s\n", real_exec_name);
    printf("Destination : %s\n\n", dest_app);

    printf("Copying bundle...\n");
    if (bundle_copy(target_app, dest_app) != 0)
        return 1;

    printf("Renaming executable...\n");
    if (bundle_rename_executable(dest_app, real_exec_name, fake_name) != 0)
        return 1;

    printf("Re-signing...\n");
    char exec_path[8192];
    snprintf(exec_path, sizeof(exec_path), "%s/Contents/MacOS/%s", dest_app, fake_name);
    if (bundle_resign(exec_path) != 0)
        return 1;

    printf("Patching Info.plist...\n");
    if (plist_patch(dest_app, fake_name) != 0)
        return 1;

    printf("\nDone.\n\n");
    printf("To launch:\n");
    printf("  open '%s'\n", dest_app);

    return 0;
}