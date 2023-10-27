//
//  main.cpp
//  Touch
//
//  Created by zero on 2023/10/26.
//

#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#import "ViewController.h"

//int SDL_Main(int argc, char *argv[]);
int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Event event;
    // 设置为横屏
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    
    window = SDL_CreateWindow(NULL, 0, 0, 320, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_SysWMinfo systemWindowInfo;
    SDL_VERSION(&systemWindowInfo.version);
    SDL_GetWindowWMInfo(window, &systemWindowInfo);
    UIWindow *appWindow = systemWindowInfo.info.uikit.window;
    UIViewController *rootVC = appWindow.rootViewController;
    appWindow.rootViewController = nil;
    UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:rootVC];
    appWindow.rootViewController = nav;
    UIStoryboard *sb = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    UIViewController *nextVC = [sb instantiateViewControllerWithIdentifier:@"Main"];
    [nav pushViewController:nextVC animated:true];
//    [appWindow.rootViewController.view addSubview:nextVC.view];
    int done = 0;
    while (!done) {
        while (SDL_PollEvent(&event) == 1) {
            switch (event.type) {
            case SDL_KEYUP:
                [nav popViewControllerAnimated:true];
                break;
            case SDL_QUIT:
                done = 1;
                break;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
