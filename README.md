# Cous Launcher
This utility allows you to patch a .app file to run under a different process name on macOS.

> [!WARNING]
> Apps that break under ad-hoc signing such as App Store apps, system apps protected under SIP, etc will not work.

## Prerequisites
This tool uses the macOS `codesign` utility, which is part of the Xcode Command Line Tools. If you don't have them already, install them with `xcode-select --install`.

---

For those looking to compile it themselves, ensure you have `make` and `clang`, which is part of the Xcode Command Line Tools. You will also require `cmake`, which you can get from `brew` or from the CMake website.

## Usage
```bash
./cl [path to .app] [new process name]
```

For example:
```bash
./cl '/Users/27corm/Desktop/Steam.app' not_steam
```

The program will output a patched version of the original .app in the current working directory.

## How it works

1. Make a copy of the .app directory
2. Finds the CFBundleExecutable.
3. Rename the executable
4. Remove the code signature of the executable.
5. Ad-hoc sign the executable.
6. Patch CFBundleName and CFBundleExecutable in Info.plist

## Build instructions
This project uses the CMake build system.

```bash
mkdir build
cd build
cmake ..
make
```

You can also find a prebuilt release for macOS in the Releases tab (ARM only). Worth noting that it should still compile fine on Intel.