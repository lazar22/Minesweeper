//
// Created by roki on 2025-11-04.
//

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL_ttf.h>

#include <score_manager.h>
#include <platform.h>
#include <renderer.h>

#define IS_DOWN(button) input.buttons[button].is_down
#define IS_PRESSED(button) (input.buttons[button].is_down && input.buttons[button].changed)
#define IS_RELEASED(button) (!input.buttons[button].is_down && input.buttons[button].changed)

typedef struct BLOCK_STATS {
    SDL_FRect rect;
    mutable SDL_Color bg;
    unsigned int mines_around{0};
    bool is_mine{false};
    mutable bool is_flagged{false};
    mutable bool is_revealed{false};
} block_stats_t;

class Game {
    SDL_Renderer *renderer;
    TTF_Font *font;
    ScoreManager *score_manager;
    platform::input::input_t input;
    Renderer *renderer_utils;

    typedef struct MOUSE_POS {
        int x;
        int y;
    } mouse_pos;

public:
    Game(SDL_Renderer *_renderer, const platform::input::input_t _input, TTF_Font *_font)
        : renderer{_renderer}, font{_font}, input{_input} {
        score_manager = new ScoreManager{platform::file::NAME};
        renderer_utils = new Renderer{renderer, font};
    };

    ~Game() = default;

public
:
    [[nodiscard]] platform::game_state::MENU_ACTION start_menu(SDL_Window *window, mouse_pos pos) const;

    [[nodiscard]] platform::game_state::MENU_ACTION game_loop(mouse_pos pos, SDL_Window *window,
                                                              double elapsed_time,
                                                              platform::game::board::board_settings_t board_size);

    void update_input(const platform::input::input_t _input) {
        input = _input;
    }

    void set_bg_color(SDL_Color color) const;

private:
    bool check_hover(SDL_FRect rect, const mouse_pos pos) const;

    void set_cursor(bool is_hovering) const;

    void get_time_stamp(double elapsed_time, char *time_stamp) const;

    void board_init(platform::game::board::board_settings_t board_size) const;

    void generate_tiles(int safe_x, int safe_y) const;

    void generate_grid() const;

    bool grid_mouse_action(const mouse_pos pos) const;

    void loop_around_tile(const int pos_x, const int pos_y) const;

    bool check_win() const;

    bool in_bounds(const int x, const int y) const;
};

#endif //GAME_H
