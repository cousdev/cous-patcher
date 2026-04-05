#import <AppKit/AppKit.h>
#include <string>

std::string open_file_picker() {
    __block std::string result;

    NSOpenPanel *panel = [NSOpenPanel openPanel];
    panel.prompt = @"Select App Bundle";
    panel.canChooseDirectories = YES;
    panel.canChooseFiles = NO;
    panel.treatsFilePackagesAsDirectories = YES;
    
    if ([panel runModal] == NSModalResponseOK) {
        result = panel.URL.path.UTF8String;
    }

    return result;
}