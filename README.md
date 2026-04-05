# Cous Patcher
This utility allows you to patch a .app file to run under a different process name on macOS.

> [!WARNING]
> Apps that break under ad-hoc signing such as App Store apps, system apps protected under SIP, etc will not work.

> [!NOTE]
> On first launch, macOS may block the app. Right-click, the app and select **Open**, then click **Open** on the dialog.
> You can also use the command line by running `xattr -d com.apple.quarantine [file_path]`.

## Prerequisites
This tool uses the macOS `codesign` utility, which is part of the Xcode Command Line Tools. If you don't have them already, install them with `xcode-select --install`.

---

For those looking to compile it themselves, ensure you have `make` and `clang`, which is part of the Xcode Command Line Tools. You will require `cmake`, which you can get from `brew` or from the CMake website. This project uses Slint for UI, which requires Rust and Cargo to be installed and present in your path.

## How it works

1. Make a copy of the .app directory
2. Finds the CFBundleExecutable.
3. Rename the executable
4. Remove the code signature of the executable.
5. Ad-hoc sign the executable.
6. Patch CFBundleName and CFBundleExecutable in Info.plist

## Build instructions
This project uses the CMake build system. First build may take some time due to compiling dependencies.

```bash
mkdir build
cd build
cmake ..
make
```

You can also find a prebuilt release for macOS in the Releases tab (ARM only). For Intel users, please build from source with the instructions listed above.