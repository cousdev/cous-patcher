#include "app.h"

#include <thread>
#include <string>

extern "C" {
#include "bundle.h"
#include "plist.h"
}

extern std::string open_file_picker();

static void append_log(slint::ComponentWeakHandle<AppWindow> weak,
                       std::string &log,
                       const std::string &line)
{
    printf("%s\n", line.c_str());

    log += line + "\n";
    std::string snapshot = log;

    slint::invoke_from_event_loop([weak, snapshot]() {
        if (auto w = weak.lock()) {
            (*w)->set_log_text(slint::SharedString(snapshot));
        }
    });
}

// Runs all patch steps on a background thread so the UI stays responsive.
static void run_patch(
    slint::SharedString app_path,
    slint::SharedString fake_name,
    slint::ComponentWeakHandle<AppWindow> weak)
{
    std::string src  = std::string(app_path);
    std::string name = std::string(fake_name);

    std::string log;

    if (src.size() < 4 || src.substr(src.size() - 4) != ".app") {
        append_log(weak, log, "Error: selected path is not a .app bundle.");
        return;
    }

    // Build destination path in the same directory as the source
    std::string dest = src;
    if (!dest.empty() && dest.back() == '/') dest.pop_back();
    auto slash = dest.rfind('/');
    std::string dir = (slash != std::string::npos) ? dest.substr(0, slash) : ".";
    dest = dir + "/" + name + ".app";

    append_log(weak, log, "Source      : " + src);
    append_log(weak, log, "Destination : " + dest);
    append_log(weak, log, "");

    append_log(weak, log, "Copying bundle...");
    if (bundle_copy(src.c_str(), dest.c_str()) != 0) {
        append_log(weak, log, "Error: failed to copy bundle.");
        return;
    }

    append_log(weak, log, "Finding executable...");
    char exec_name[256];
    if (bundle_find_executable(src.c_str(), exec_name, sizeof(exec_name)) != 0) {
        append_log(weak, log, "Error: could not find executable.");
        return;
    }
    append_log(weak, log, std::string("  Found: ") + exec_name);

    append_log(weak, log, "Renaming executable...");
    if (bundle_rename_executable(dest.c_str(), exec_name, name.c_str()) != 0) {
        append_log(weak, log, "Error: failed to rename executable.");
        return;
    }

    append_log(weak, log, "Re-signing...");
    std::string exec_path = dest + "/Contents/MacOS/" + name;
    if (bundle_resign(exec_path.c_str()) != 0) {
        append_log(weak, log, "Error: failed to re-sign.");
        return;
    }

    append_log(weak, log, "Patching Info.plist...");
    if (plist_patch(dest.c_str(), name.c_str()) != 0) {
        append_log(weak, log, "Error: failed to patch Info.plist.");
        return;
    }

    append_log(weak, log, "Output: " + dest);
    append_log(weak, log, "Patch successful!");
}

int main() {
    auto window = AppWindow::create();

    window->on_browse_clicked([&]() {
        std::string path = open_file_picker();
        if (!path.empty())
            window->set_app_path(slint::SharedString(path));
    });

    window->on_patch_clicked([&](slint::SharedString app_path, slint::SharedString fake_name) {
        window->set_log_text(slint::SharedString(""));
        window->set_patching(true);

        slint::ComponentWeakHandle<AppWindow> weak(window);
        std::thread([app_path, fake_name, weak]() {
            run_patch(app_path, fake_name, weak);
        }).detach();
    });

    window->run();
    return 0;
}