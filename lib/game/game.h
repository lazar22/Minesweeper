//
// Created by roki on 2025-11-04.
//

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL_ttf.h>

#include <platform.h>

typedef struct CIRCLE {
    float center_x;
    float center_y;
    float r;
} circle_t;

class Game {
    SDL_Renderer *renderer;
    TTF_Font *font;

    typedef struct MOUSE_POS {
        int x;
        int y;
    } mouse_pos;

public:
    Game(SDL_Renderer *_renderer, TTF_Font *_font) : renderer{_renderer}, font{_font} {
    };

    ~Game() = default;

public
:
    void start_menu(const mouse_pos pos) const;

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
};

#endif //GAME_H
