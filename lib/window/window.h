//
// Created by roki on 2025-12-02.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include <platform.h>

class Window {
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    TTF_Font *font{nullptr};

    SDL_DisplayMode window_size;

public:
    Window(int width, int height, const char *title);

    ~Window();

public:
    SDL_Window *get_window() const;

    SDL_Renderer *get_renderer() const;

    TTF_Font *get_font() const;
};

#endif //WINDOW_H
