//
// Created by roki on 2025-11-04.
//

#include <SDL2/SDL.h>

#include <platform.h>

#include "game.h"

static int mouse_x, mouse_y;
static SDL_DisplayMode window_size;

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    if (TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF: %s", TTF_GetError());
    }

    SDL_GetCurrentDisplayMode(0, &window_size);

    const int window_x = (window_size.w - platform::window::WIDTH) / 2;
    const int window_y = (window_size.h - platform::window::HEIGHT) / 2;


    SDL_Window *window = SDL_CreateWindow(platform::window::TITLE,
                                          window_x, window_y,
                                          platform::window::WIDTH, platform::window::HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    const char *fontPath = "assets/font/ChrustyRock-ORLA.ttf";
    TTF_Font *font = TTF_OpenFont(fontPath, platform::font::TITLE_SIZE);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont('%s') failed: %s", fontPath, TTF_GetError());
        SDL_Log("BasePath: %s", SDL_GetBasePath() ? : "(null)");
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event event;

    bool is_running = true;

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }

            switch (event.type) {
                // case SDL_WINDOWEVENT: {
                //     if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                //         SDL_GetCurrentDisplayMode(0, &window_size);
                //         SDL_RenderClear(renderer);
                //     }
                //
                //     break;
                // }

                case SDL_MOUSEMOTION: {
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    break;
                }

                default: break;
            }
        }

        Game game{renderer, font};
        game.set_bg_color(platform::window::COLOR);

        SDL_RenderClear(renderer);

        game.start_menu({mouse_x, mouse_y});

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
