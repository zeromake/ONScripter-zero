#import <TargetConditionals.h>
#if TARGET_OS_OSX
#import <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#import <Foundation/NSArray.h>
#endif
#include "ONScripter.h"

#if defined(IOS)
extern "C" void playVideoIOS(const char *filename, bool click_flag,
                             bool loop_flag) {
    // NSString *str = [[NSString alloc] initWithUTF8String:filename];
    // id obj = [MoviePlayer alloc];
    // [[obj init] play:str click : click_flag loop : loop_flag];
    // [obj release];
}
#endif

void onscripter_init(ONScripter *ons, int argc, char *argv[]) {
#if defined(IOS)
#if defined(HAVE_CONTENTS)
    if ([[[DataCopier alloc] init] copy]) exit(-1);
#endif
    // scripts and archives are stored under /Library/Caches
    // NSArray *cpaths = NSSearchPathForDirectoriesInDomains(
    //     NSCachesDirectory, NSUserDomainMask, YES);
    // NSString *cpath =
    //     [[cpaths objectAtIndex:0] stringByAppendingPathComponent:@"ONS"];
    // char filename[256];
    // strcpy(filename, [cpath UTF8String]);
    // ons->setArchivePath(filename);
    // // output files are stored under /Documents
    // NSArray *dpaths = NSSearchPathForDirectoriesInDomains(
    //     NSDocumentDirectory, NSUserDomainMask, YES);
    // NSString *dpath =
    //     [[dpaths objectAtIndex:0] stringByAppendingPathComponent:@"ONS"];
    // strcpy(filename, [dpath UTF8String]);
    // ons->setSaveDir(filename);

#if defined(ZIP_URL)
    if ([[[DataDownloader alloc] init] download]) exit(-1);
#endif

#if defined(USE_SELECTOR)
    scripts and archives are stored under /Library/Caches
    cpath =
        [[[ScriptSelector alloc] initWithStyle:UITableViewStylePlain] select];
    strcpy(filename, [cpath UTF8String]);
    ons->setArchivePath(filename);

    // output files are stored under /Documents
    dpath = [[dpaths objectAtIndex:0]
        stringByAppendingPathComponent:[cpath lastPathComponent]];
    NSFileManager *fm = [NSFileManager defaultManager];
    [fm createDirectoryAtPath:dpath
        withIntermediateDirectories:YES
                         attributes:nil
                              error:nil];
    strcpy(filename, [dpath UTF8String]);
    ons->setSaveDir(filename);
#endif

#if defined(RENDER_FONT_OUTLINE)
    ons->renderFontOutline();
#endif
#endif

#if TARGET_OS_OSX
//     printf("argc: %d\n", argc);
//     if (argc == 1) {
//         NSOpenPanel *panel = [NSOpenPanel openPanel];
//         panel.allowsMultipleSelection = NO;
//         panel.canChooseFiles = NO;
//         panel.canChooseDirectories = YES;
//         __block volatile BOOL is_running = YES;
//         [panel beginSheetModalForWindow:window completionHandler:^(NSModalResponse result) {
//             if (panel.URL != nil && result == NSModalResponseOK) {
//                 printf("panel select: %s\n", [panel.URL.path UTF8String]);
//             }
//             printf("panel done0\n");
//             is_running = NO;
//         }];
//         while(is_running)
//             [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
//         printf("panel done\n");
//     }
#endif

#if TARGET_OS_OSX
    CGFloat dpiScale = NSScreen.mainScreen.backingScaleFactor;
#else
    CGFloat dpiScale = 0.0f;
#endif
    if (dpiScale > 1.0f) {
        ons->force_render_ratio1 = 1;
        ons->force_render_ratio2 = (int)dpiScale;
    }
}