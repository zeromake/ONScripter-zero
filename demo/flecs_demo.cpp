#include <SDL2/SDL.h>
#include <flecs.h>
#include <iostream>
#include <thread>

struct Position {
  float x, y;
};

struct Eventer {
  SDL_Event event;
};

struct App {
  SDL_Window* window;
  SDL_Renderer* renderer;
};

int main(int argc, char *argv[]) {
    flecs::world ecs;
    SDL_bool exit = SDL_FALSE;
    ecs.system<Eventer>().each([&exit](Eventer& app){
        SDL_WaitEvent(&app.event);
        if (app.event.type == SDL_QUIT) {
            exit = SDL_TRUE;
        }
    });

    auto appEntity = ecs.entity().set([](App& app, Eventer& t) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_ALLOW_HIGHDPI, &app.window, &app.renderer);
    });
    std::cout << "main thread id: " << std::this_thread::get_id() << "\n";
    ecs.set_threads(10);
    while (ecs.progress(16) && !exit) {
    }
}