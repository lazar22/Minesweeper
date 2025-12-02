//
// Created by roki on 2025-12-02.
//

#include "window.h"

Window::Window(const int width, const int height, const char *title) {
    if (TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF: %s", TTF_GetError());
    }

    SDL_GetCurrentDisplayMode(0, &window_size);

    const int window_x = (window_size.w - width) / 2;
    const int window_y = (window_size.h - height) / 2;


    window = SDL_CreateWindow(title,
                              window_x, window_y,
                              width, height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    const char *fontPath = "assets/font/04b_25__.ttf";
    font = TTF_OpenFont(fontPath, platform::font::TITLE_SIZE);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont('%s') failed: %s", fontPath, TTF_GetError());
        SDL_Log("BasePath: %s", SDL_GetBasePath() ? : "(null)");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_GL_SetSwapInterval(1);
}

Window::~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();
}

SDL_Window *Window::get_window() const {
    return window;
}

SDL_Renderer *Window::get_renderer() const {
    return renderer;
}

TTF_Font *Window::get_font() const {
    return font;
}
