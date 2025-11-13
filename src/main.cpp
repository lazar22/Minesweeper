//
// Created by roki on 2025-11-04.
//

#include <SDL2/SDL.h>

#include <platform.h>
#include "game.h"

static platform::input::input_t input;

static int mouse_x, mouse_y;
static SDL_DisplayMode window_size;

static uint8_t current_state = platform::game_state::TITLE;

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

    const char *fontPath = "assets/font/04b_25__.ttf";
    TTF_Font *font = TTF_OpenFont(fontPath, platform::font::TITLE_SIZE);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont('%s') failed: %s", fontPath, TTF_GetError());
        SDL_Log("BasePath: %s", SDL_GetBasePath() ? : "(null)");
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_GL_SetSwapInterval(1);

    SDL_Event event;

    bool is_running = true;

    uint32_t start_timer = SDL_GetTicks();
    while (is_running) {
        for (int i = 0; i < platform::input::BUTTON_COUNT; i++) {
            input.buttons[i].changed = false;
        }

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
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {
                    const bool is_down = (event.type == SDL_MOUSEBUTTONDOWN);

                    switch (event.button.button) {
                        READ_KEY(platform::input::MOUSE_LEFT, SDL_BUTTON_LEFT);
                        READ_KEY(platform::input::MOUSE_RIGHT, SDL_BUTTON_RIGHT);
                        default: break;
                    }
                }

                default: break;
            }
        }

        platform::game_state::MENU_ACTION action{platform::game_state::TITLE};

        Game game{renderer, input, font};
        game.set_bg_color(platform::window::COLOR);

        SDL_RenderClear(renderer);

        if (current_state == platform::game_state::TITLE) {
            switch (game.start_menu({mouse_x, mouse_y})) {
                case platform::game_state::PLAYING: {
                    current_state = platform::game_state::PLAYING;
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                    break;
                }
                case platform::game_state::QUIT: {
                    is_running = false;
                    break;
                }

                default: break;
            }
        }

        if (current_state == platform::game_state::PLAYING) {
            const uint32_t current_time = SDL_GetTicks();
            const double elapsed_time = (current_time - start_timer) / 1000.0; // decided by 1000 to get seconds

            switch (game.game_loop({mouse_x, mouse_y}, window, elapsed_time, {8, 8, 10})) {
                case platform::game_state::PLAYING: {
                    break;
                }
                case platform::game_state::TITLE: {
                    current_state = platform::game_state::TITLE;
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                    break;
                }
                case platform::game_state::QUIT: {
                    is_running = false;
                    break;
                }
                default: break;
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
