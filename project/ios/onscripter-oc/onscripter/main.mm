//
//  main.cpp
//  Touch
//
//  Created by zero on 2023/10/26.
//

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#import "ViewController.h"

extern "C" int onscripter_main(const int argc, const char *argv[]);

static int __window_size[2];

extern "C" void onscripter_window_pixels(int *width, int *height) {
    *width = __window_size[0];
    *height = __window_size[1];
}

#undef main

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Event event;
    // 设置为横屏
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    window = SDL_CreateWindow(NULL, 0, 0, 320, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
    int w = 0;
    int h = 0;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    __window_size[0] = w;
    __window_size[1] = h;
    SDL_SysWMinfo systemWindowInfo;
    SDL_VERSION(&systemWindowInfo.version);
    SDL_GetWindowWMInfo(window, &systemWindowInfo);
    UIWindow *appWindow = systemWindowInfo.info.uikit.window;
    UIViewController *rootVC = appWindow.rootViewController;
    char script_dir[4096]{0};
    {
        appWindow.rootViewController = nil;
        // UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:rootVC];
        // appWindow.rootViewController = nav;
        UIStoryboard *sb = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
        ViewController *nextVC = [sb instantiateViewControllerWithIdentifier:@"Main"];
        // [nav pushViewController:nextVC animated:false];
        appWindow.rootViewController = nextVC;
        int done = 0;
        while (!done) {
            while (SDL_PollEvent(&event) == 1) {
                switch (event.type) {
                case SDL_KEYUP:
                    // [nav popViewControllerAnimated:false];
                    break;
                case SDL_QUIT:
                    done = 1;
                    break;
                }
            }
        }
        // [nav popViewControllerAnimated:false];
        appWindow.rootViewController = nil;
        strcpy(script_dir, [[nextVC get_script_dir] UTF8String]);
        printf("script_dir: %s\n", script_dir);
    }
    appWindow.rootViewController = rootVC;
    SDL_DestroyWindow(window);
    SDL_Quit();
    const char *ons_argv[] = {"onscripter", "--scale-window", script_dir};
    int ons_argc = 3;
    return onscripter_main(ons_argc, ons_argv);
}
