//
// Created by roki on 2025-12-02.
//

#include "window.h"
#include "platform.h"
#include "game.h"

// Use the same input object defined in main.cpp
extern platform::input::input_t input;

Window::Window(const int width, const int height, const char* title)
{
    if (TTF_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF: %s", TTF_GetError());
    }

    SDL_GetCurrentDisplayMode(0, &window_size);

    const int window_x = (window_size.w - width) / 2;
    const int window_y = (window_size.h - height) / 2;


    window = SDL_CreateWindow(title,
                              window_x, window_y,
                              width, height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    const auto fontPath = "assets/font/04b_25__.ttf";
    font = TTF_OpenFont(fontPath, platform::font::TITLE_SIZE);
    if (!font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont('%s') failed: %s", fontPath, TTF_GetError());
        SDL_Log("BasePath: %s", SDL_GetBasePath() ? SDL_GetBasePath() : "(null)");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_GL_SetSwapInterval(1);
}

Window::~Window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();
}

void Window::register_events(void)
{
    for (int i = 0; i < platform::input::BUTTON_COUNT; i++)
    {
        input.buttons[i].changed = false;
    }

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            is_running = false;
        }

        switch (event.type)
        {
        // case SDL_WINDOWEVENT: {
        //     if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        //         SDL_GetCurrentDisplayMode(0, &window_size);
        //         SDL_RenderClear(renderer);
        //     }
        //
        //     break;
        // }

        case SDL_MOUSEMOTION:
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                break;
            }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                const bool is_down = (event.type == SDL_MOUSEBUTTONDOWN);

                switch (event.button.button)
                {
                READ_KEY(platform::input::MOUSE_LEFT, SDL_BUTTON_LEFT);
                READ_KEY(platform::input::MOUSE_RIGHT, SDL_BUTTON_RIGHT);
                default: break;
                }
            }

        default: break;
        }
    }
}

void Window::main_loop(const std::vector<func_t>& functions)
{
    while (is_running)
    {
        register_events();

        // TODO: Add pick board size screen
        for (const auto& func : functions)
        {
            func();
        }

        SDL_RenderPresent(get_renderer());
    }
}

void Window::set_is_running(const bool _is_running)
{
    is_running = _is_running;
}

void Window::set_start_timer(const uint32_t _start_timer)
{
    start_timer = _start_timer;
}


SDL_Window* Window::get_window() const
{
    return window;
}

SDL_Renderer* Window::get_renderer() const
{
    return renderer;
}

TTF_Font* Window::get_font() const
{
    return font;
}

uint32_t Window::get_start_timer() const
{
    return start_timer;
}

int Window::get_mouse_x() const
{
    return mouse_x;
}

int Window::get_mouse_y() const
{
    return mouse_y;
}
