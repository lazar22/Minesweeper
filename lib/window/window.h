//
// Created by roki on 2025-12-02.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include <vector>

#include <platform.h>

using func_t = void(*)(void);

class Window
{
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    TTF_Font* font{nullptr};

    SDL_DisplayMode window_size{};
    SDL_Event event{};

    int mouse_x{};
    int mouse_y{};

    bool is_running{true};

    uint32_t start_timer{SDL_GetTicks()};

public:
    Window(const int width, const int height, const char* font_path, const char* title);

    ~Window();

public:
    void register_events(void);

    void main_loop(const std::vector<func_t>& functions);

    void set_is_running(bool _is_running);

    void set_start_timer(uint32_t _start_timer);

public:
    [[nodiscard]] SDL_Window* get_window(void) const;

    [[nodiscard]] SDL_Renderer* get_renderer(void) const;

    [[nodiscard]] TTF_Font* get_font(void) const;

    [[nodiscard]] uint32_t get_start_timer(void) const;

    [[nodiscard]] int get_mouse_x(void) const;

    [[nodiscard]] int get_mouse_y(void) const;
};

#endif //WINDOW_H
