//
// Created by roki on 2025-11-04.
//

#include <SDL2/SDL.h>

#include <platform.h>
#include "game.h"
#include "window.h"

platform::input::input_t input;

static uint8_t current_state = platform::game_state::TITLE;

Game* game{nullptr};
Window* main_window{nullptr};

void update_input(void)
{
    game->update_input(input);
    game->set_bg_color(platform::window::COLOR);
    SDL_RenderClear(main_window->get_renderer());
}

void render_title_screen(void)
{
    if (current_state == platform::game_state::TITLE)
    {
        switch (game->start_menu(main_window->get_window(), {main_window->get_mouse_x(), main_window->get_mouse_y()}))
        {
        case platform::game_state::PLAYING:
            {
                current_state = platform::game_state::PLAYING;
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                main_window->set_start_timer(SDL_GetTicks());
                break;
            }
        case platform::game_state::QUIT:
            {
                main_window->set_is_running(false);
                break;
            }

        default: break;
        }
    }
}

void render_game_screen(void)
{
    if (current_state == platform::game_state::PLAYING)
    {
        const uint32_t current_time = SDL_GetTicks();
        const double elapsed_time = (current_time - main_window->get_start_timer()) / 1000.0;

        switch (game->game_loop({main_window->get_mouse_x(), main_window->get_mouse_y()}, main_window->get_window(),
                                elapsed_time, {8, 8, 10}))
        {
        case platform::game_state::PLAYING:
            {
                break;
            }
        case platform::game_state::TITLE:
            {
                current_state = platform::game_state::TITLE;
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                break;
            }
        case platform::game_state::QUIT:
            {
                main_window->set_is_running(false);
                break;
            }
        default: break;
        }
    }
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    main_window = new Window{platform::window::WIDTH, platform::window::HEIGHT, platform::window::TITLE};

    if (main_window->get_window() == nullptr ||
        main_window->get_renderer() == nullptr ||
        main_window->get_font() == nullptr ||
        main_window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    game = new Game{main_window->get_renderer(), input, main_window->get_font()};
    if (game == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create game: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    const std::vector<func_t> functions = {
        update_input,
        render_title_screen,
        render_game_screen
    };
    main_window->main_loop(functions);

    return 0;
}
