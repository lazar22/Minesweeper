//
// Created by roki on 2025-11-04.
//

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL_ttf.h>

#include <platform.h>

#define IS_DOWN(button) input.buttons[button].is_down
#define IS_PRESSED(button) (input.buttons[button].is_down && input.buttons[button].changed)
#define IS_RELEASED(button) (!input.buttons[button].is_down && input.buttons[button].changed)

typedef struct CIRCLE {
    float center_x;
    float center_y;
    float r;
} circle_t;

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
    platform::input::input_t input;

    typedef struct MOUSE_POS {
        int x;
        int y;
    } mouse_pos;

public:
    Game(SDL_Renderer *_renderer, const platform::input::input_t _input, TTF_Font *_font)
        : renderer{_renderer}, font{_font}, input{_input} {
    };

    ~Game() = default;

public
:
    [[nodiscard]] platform::game_state::MENU_ACTION start_menu(const mouse_pos pos) const;

    [[nodiscard]] platform::game_state::MENU_ACTION game_loop(const mouse_pos pos, SDL_Window *window,
                                                              double elapsed_time,
                                                              platform::game::board::board_settings_t board_size);

    void set_bg_color(SDL_Color color) const;

private
:
    void draw_rect(SDL_FRect rect, SDL_Color color) const;

    void draw_txt(SDL_Rect pos, SDL_Color color, const char *txt) const;

    void draw_circle(circle_t circle, SDL_Color color) const;

    void draw_filled_circle(circle_t circle, SDL_Color color) const;

    void draw_rounded_rect(SDL_FRect rect, float r, SDL_Color color) const;

    bool check_hover(SDL_FRect rect, const mouse_pos pos) const;

    void set_cursor(bool is_hovering) const;

    void get_time_stamp(double elapsed_time, char *time_stamp) const;

    void board_init(platform::game::board::board_settings_t board_size) const;

    void generate_grid() const;

    bool grid_mouse_action(const mouse_pos pos);

    static void loop_around_tile(const int pos_x, const int pos_y);

    bool check_win() const;
};

#endif //GAME_H
