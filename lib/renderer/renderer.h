//
// Created by roki on 2025-12-02.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

typedef struct CIRCLE
{
    float center_x;
    float center_y;
    float r;
} circle_t;

class Renderer
{
    SDL_Renderer* renderer;
    TTF_Font* font;

public:
    Renderer(SDL_Renderer* _renderer, TTF_Font* _font) : renderer{_renderer}, font{_font}
    {
    };

    ~Renderer() = default;

public:
    void draw_rect(SDL_FRect rect, SDL_Color color) const;

    void draw_txt(SDL_Rect pos, SDL_Color color, const char* txt) const;

    void draw_txt_centered(SDL_Rect bounds, SDL_Color color, const char* txt, float user_scale) const;

    void draw_circle(circle_t circle, SDL_Color color) const;

    void draw_filled_circle(circle_t circle, SDL_Color color) const;

    void draw_rounded_rect(SDL_FRect rect, float r, SDL_Color color) const;
};

#endif //RENDERER_H
