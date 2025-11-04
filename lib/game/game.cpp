//
// Created by roki on 2025-11-04.
//

#include "game.h"

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>

void Game::start_menu(const mouse_pos pos) const {
    const int btn_h = {100};
    const int btn_w = {200};
    const int btn_offset = {20};

    const int middle_x = (platform::window::WIDTH - btn_w) / 2;
    const int middle_y = (platform::window::HEIGHT - btn_h) / 2;

    SDL_FRect start_btn = {middle_x, middle_y, btn_w, btn_h};
    SDL_FRect quit_btn = {middle_x, middle_y + btn_offset + btn_h, btn_w, btn_h};

    draw_rect(start_btn, {255, 0, 0, 255});
    draw_rect(quit_btn, {255, 0, 0, 255});

    draw_txt({
                 static_cast<int>(start_btn.x), static_cast<int>(start_btn.y),
                 static_cast<int>(start_btn.w), static_cast<int>(start_btn.h)
             },
             {0, 0, 0, 255},
             "Start");

    draw_txt({
                 static_cast<int>(quit_btn.x), static_cast<int>(quit_btn.y),
                 static_cast<int>(quit_btn.w), static_cast<int>(quit_btn.h)
             },
             {0, 0, 0, 255},
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

void Game::set_bg_color(const SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

bool Game::check_hover(SDL_FRect rect, const mouse_pos pos) const {
    bool is_hovering = false;

    if (rect.x + rect.w > pos.x && rect.x < pos.x &&
        rect.y + rect.h > pos.y && rect.y < pos.y) {
        is_hovering = true;
    } else {
        is_hovering = false;
    }

    return is_hovering;
}

void Game::set_cursor(bool is_hovering) const {
    if (is_hovering) {
        SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(cursor);
    } else {
        SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_SetCursor(cursor);
    }
}
