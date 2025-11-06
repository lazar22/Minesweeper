//
// Created by roki on 2025-11-04.
//

#include "game.h"

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <algorithm>

void Game::start_menu(const mouse_pos pos) const {
    constexpr int btn_h = {100};
    constexpr int btn_w = {200};
    constexpr int btn_offset = {20};

    constexpr int middle_x = (platform::window::WIDTH - btn_w) / 2;
    constexpr int middle_y = (platform::window::HEIGHT - btn_h) / 2;

    constexpr SDL_FRect start_btn = {middle_x, middle_y, btn_w, btn_h};
    constexpr SDL_FRect quit_btn = {middle_x, middle_y + btn_offset + btn_h, btn_w, btn_h};

    draw_rounded_rect(start_btn, 10, platform::font::color::BG);
    draw_rounded_rect(quit_btn, 10, platform::font::color::BG);

    draw_txt({
                 static_cast<int>(start_btn.x), static_cast<int>(start_btn.y),
                 static_cast<int>(start_btn.w), static_cast<int>(start_btn.h)
             },
             platform::font::color::MAIN,
             "Start");

    draw_txt({
                 static_cast<int>(quit_btn.x), static_cast<int>(quit_btn.y),
                 static_cast<int>(quit_btn.w), static_cast<int>(quit_btn.h)
             },
             platform::font::color::MAIN,
             "Quit");

    if (check_hover(start_btn, pos) || check_hover(quit_btn, pos)) {
        set_cursor(true);
    } else {
        set_cursor(false);
    }
}

void Game::draw_rect(const SDL_FRect rect, const SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRectF(renderer, &rect);
}

void Game::draw_txt(SDL_Rect pos, const SDL_Color color, const char *txt) const {
    SDL_Surface *surface_msg = TTF_RenderText_Solid(font, txt, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, surface_msg);
    SDL_RenderCopy(renderer, text_texture, nullptr, &pos);

    SDL_FreeSurface(surface_msg);
    SDL_DestroyTexture(text_texture);
}

void Game::draw_circle(const circle_t circle, const SDL_Color color) const {
    float x = circle.r - 1;
    float y = 0;

    float t_x = 1;
    float t_y = 1;

    float err = (t_x - (circle.r * 2));

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    while (x > y) {
        SDL_RenderDrawPointF(renderer, circle.center_x + x, circle.center_y - y);
        SDL_RenderDrawPointF(renderer, circle.center_x + x, circle.center_y + y);
        SDL_RenderDrawPointF(renderer, circle.center_x - x, circle.center_y - y);
        SDL_RenderDrawPointF(renderer, circle.center_x - x, circle.center_y + y);

        SDL_RenderDrawPointF(renderer, circle.center_x + y, circle.center_y - x);
        SDL_RenderDrawPointF(renderer, circle.center_x + y, circle.center_y + x);
        SDL_RenderDrawPointF(renderer, circle.center_x - y, circle.center_y - x);
        SDL_RenderDrawPointF(renderer, circle.center_x - y, circle.center_y + x);

        if (err <= 0) {
            y++;
            err += t_y;
            t_y += 2;
        }

        if (err > 0) {
            x--;
            err += (t_x - (circle.r * 2));
            t_x += 2;
        }
    }
}

void Game::draw_filled_circle(const circle_t circle, const SDL_Color color) const {
    float x = circle.r - 1;
    float y = 0;

    float t_x = 1;
    float t_y = 1;

    float err = (t_x - (circle.r * 2));

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const float center_x = circle.center_x;
    const float center_y = circle.center_y;

    while (x >= y) {
        SDL_RenderDrawLineF(renderer, center_x - x, center_y + y, center_x + x, center_y + y);
        SDL_RenderDrawLineF(renderer, center_x - x, center_y - y, center_x + x, center_y - y);
        SDL_RenderDrawLineF(renderer, center_x - y, center_y + x, center_x + y, center_y + x);
        SDL_RenderDrawLineF(renderer, center_x - y, center_y - x, center_x + y, center_y - x);

        if (err <= 0) {
            y++;
            err += t_y;
            t_y += 2;
        }

        if (err > 0) {
            x--;
            err += (t_x - (circle.r * 2));
            t_x += 2;
        }
    }
}

void Game::draw_rounded_rect(const SDL_FRect rect, const float r, const SDL_Color color) const {
    if (rect.w <= 0 || rect.h <= 0) return;

    const float max_r = std::min(rect.w, rect.h) * 0.5f;
    const float rad = std::max(0.0f, std::min(r, max_r));

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const SDL_FRect center = {rect.x + rad, rect.y, rect.w - 2.0f * rad, rect.h};
    if (center.w > 0 && center.h > 0) SDL_RenderFillRectF(renderer, &center);

    const SDL_FRect left_strip = {rect.x, rect.y + rad, rad, rect.h - 2.0f * rad};
    if (left_strip.w > 0 && left_strip.h > 0) SDL_RenderFillRectF(renderer, &left_strip);

    const SDL_FRect right_strip = {rect.x + rect.w - rad, rect.y + rad, rad, rect.h - 2.0f * rad};
    if (right_strip.w > 0 && right_strip.h > 0) SDL_RenderFillRectF(renderer, &right_strip);

    if (rad <= 0.0f) { return; } {
        const SDL_Rect clip = {
            static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rad), static_cast<int>(rad)
        };
        SDL_RenderSetClipRect(renderer, &clip);
        draw_filled_circle({rect.x + rad, rect.y + rad, rad}, color);
        SDL_RenderSetClipRect(renderer, nullptr);
    } {
        const SDL_Rect clip = {
            static_cast<int>(rect.x + rect.w - rad), static_cast<int>(rect.y), static_cast<int>(rad),
            static_cast<int>(rad)
        };
        SDL_RenderSetClipRect(renderer, &clip);
        draw_filled_circle({rect.x + rect.w - rad, rect.y + rad, rad}, color);
        SDL_RenderSetClipRect(renderer, nullptr);
    } {
        const SDL_Rect clip = {
            static_cast<int>(rect.x), static_cast<int>(rect.y + rect.h - rad), static_cast<int>(rad),
            static_cast<int>(rad)
        };
        SDL_RenderSetClipRect(renderer, &clip);
        draw_filled_circle({rect.x + rad, rect.y + rect.h - rad, rad}, color);
        SDL_RenderSetClipRect(renderer, nullptr);
    } {
        const SDL_Rect clip = {
            static_cast<int>(rect.x + rect.w - rad), static_cast<int>(rect.y + rect.h - rad), static_cast<int>(rad),
            static_cast<int>(rad)
        };
        SDL_RenderSetClipRect(renderer, &clip);
        draw_filled_circle({rect.x + rect.w - rad, rect.y + rect.h - rad, rad}, color);
        SDL_RenderSetClipRect(renderer, nullptr);
    }
}


void Game::set_bg_color(const SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

bool Game::check_hover(const SDL_FRect rect, const mouse_pos pos) const {
    bool is_hovering = false;

    if (rect.x + rect.w > pos.x && rect.x < pos.x &&
        rect.y + rect.h > pos.y && rect.y < pos.y) {
        is_hovering = true;
    } else {
        is_hovering = false;
    }

    return is_hovering;
}

void Game::set_cursor(const bool is_hovering) const {
    if (is_hovering) {
        SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(cursor);
    } else {
        SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_SetCursor(cursor);
    }
}
