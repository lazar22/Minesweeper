//
// Created by roki on 2025-11-04.
//

#include <SDL2/SDL.h>

#include <platform.h>
#include "game.h"
#include "window.h"

static platform::input::input_t input;

static int mouse_x, mouse_y;

static uint8_t current_state = platform::game_state::TITLE;

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Event event;

    bool is_running = true;

    Window main_window{platform::window::WIDTH, platform::window::HEIGHT, platform::window::TITLE};

    uint32_t start_timer = SDL_GetTicks();
    Game game{main_window.get_renderer(), input, main_window.get_font()};

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

        game.update_input(input);
        game.set_bg_color(platform::window::COLOR);

        SDL_RenderClear(main_window.get_renderer());

        // Title Screen
        if (current_state == platform::game_state::TITLE) {
            switch (game.start_menu(main_window.get_window(), {mouse_x, mouse_y})) {
                case platform::game_state::PLAYING: {
                    current_state = platform::game_state::PLAYING;
                    SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                    start_timer = SDL_GetTicks();
                    break;
                }
                case platform::game_state::QUIT: {
                    is_running = false;
                    break;
                }

                default: break;
            }
        }

        // TODO: Add pick board size screen

        // In game screen
        if (current_state == platform::game_state::PLAYING) {
            const uint32_t current_time = SDL_GetTicks();
            const double elapsed_time = (current_time - start_timer) / 1000.0; // decided by 1000 to get seconds

            switch (game.game_loop({mouse_x, mouse_y}, main_window.get_window(), elapsed_time, {8, 8, 10})) {
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

        SDL_RenderPresent(main_window.get_renderer());
    }

    return 0;
}
