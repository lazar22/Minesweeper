//
// Created by roki on 2025-11-04.
//

#include "game.h"

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <algorithm>
#include <string>
#include <limits>
#include <queue>

static SDL_Color current_start_color = platform::font::color::BG;
static SDL_Color current_quit_color = platform::font::color::BG;

static SDL_FRect current_start_rect;
static SDL_FRect current_quit_rect;

static float grid_w;
static float grid_h;

static std::vector<std::vector<block_stats_t> > board;

static bool ignore_left_click_until_release{false};
static bool init_generation{false};

static int amount_of_mines{0};

platform::game_state::MENU_ACTION Game::start_menu(const mouse_pos pos) const {
    constexpr int btn_h = {100};
    constexpr int btn_w = {200};
    constexpr int btn_offset = {20};

    constexpr int border_r = {10};

    constexpr int middle_x = (platform::window::WIDTH - btn_w) / 2;
    constexpr int middle_y = (platform::window::HEIGHT - btn_h) / 2;

    constexpr SDL_FRect start_btn = {middle_x, middle_y, btn_w, btn_h};
    constexpr SDL_FRect quit_btn = {middle_x, middle_y + btn_offset + btn_h, btn_w, btn_h};

    draw_rounded_rect(start_btn, border_r, current_start_color);
    draw_rounded_rect(quit_btn, border_r, current_quit_color);

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

    const bool is_start_hover = check_hover(start_btn, pos);
    const bool is_quit_hover = check_hover(quit_btn, pos);

    if (is_start_hover) {
        current_start_color = platform::font::color::BG_HOVER;

        current_start_rect.w = start_btn.w + 10;
        current_start_rect.h = start_btn.h + 10;

        if (IS_PRESSED(platform::input::MOUSE_LEFT)) {
            init_generation = true;
            ignore_left_click_until_release = true;
            return platform::game_state::PLAYING;
        }
    }

    if (is_quit_hover) {
        current_quit_color = platform::font::color::BG_HOVER;

        current_quit_rect.w = quit_btn.w + 10;
        current_quit_rect.h = quit_btn.h + 10;

        if (IS_PRESSED(platform::input::MOUSE_LEFT)) {
            return platform::game_state::QUIT;
        }
    }

    if (is_start_hover || is_quit_hover) {
        set_cursor(true);
    } else {
        set_cursor(false);

        current_start_color = platform::font::color::BG;
        current_quit_color = platform::font::color::BG;

        current_start_rect = start_btn;
        current_quit_rect = quit_btn;
    }

    return platform::game_state::TITLE;
}

platform::game_state::MENU_ACTION Game::game_loop(const mouse_pos pos, SDL_Window *window,
                                                  const double elapsed_time,
                                                  const platform::game::board::board_settings_t board_size) {
    char temp[sizeof(std::to_string(DBL_MAX))];

    get_time_stamp(elapsed_time, temp);

    if (init_generation) {
        board_init(board_size);
        init_generation = false;
    }

    generate_grid();

    // If true, you lost the game
    if (grid_mouse_action(pos)) {
        // TODO: Add a wait before going to the title screen or have a more user friendly output that you lost the game
        return platform::game_state::TITLE;
    }

    const std::string title = std::string(platform::window::TITLE) + " " + temp;
    SDL_SetWindowTitle(window, title.c_str());

    // If true, you won the game
    if (check_win()) {
        // TODO: check if the time is in the best of 10, and if it is return to the platform::game_state::SCORE
        return platform::game_state::TITLE;
    }

    return platform::game_state::PLAYING;
}


void Game::draw_rect(const SDL_FRect rect, const SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRectF(renderer, &rect);
}

void Game::draw_txt(const SDL_Rect pos, const SDL_Color color, const char *txt) const {
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

void Game::get_time_stamp(const double elapsed_time, char *time_stamp) const {
    if (elapsed_time < 60) {
        const int seconds = static_cast<const int>(elapsed_time);
        const int milliseconds = static_cast<const int>((elapsed_time - seconds) * 100.0);

        sprintf(time_stamp, "(%d:%02d) seconds", seconds, milliseconds);
    } else if (elapsed_time < 3600) {
        const int total = static_cast<const int>(elapsed_time);
        const int minutes = total / 60;
        const int seconds = total % 60;

        sprintf(time_stamp, "(%d:%02d) minutes", minutes, seconds);
    } else {
        const int total = static_cast<const int>(elapsed_time);
        const int hours = total / 3600;
        const int minutes = (total % 3600) / 60;

        sprintf(time_stamp, "(%d:%02d) hours", hours, minutes);
    }
}

void Game::board_init(const platform::game::board::board_settings_t board_size) const {
    const int cols = board_size.w;
    const int rows = board_size.h;

    board.assign(rows, std::vector<block_stats_t>(cols));

    constexpr float cell = platform::game::block::SIZE;
    constexpr float gap = platform::game::block::OFFSET;

    grid_w = static_cast<float>(cols) * cell + static_cast<float>(cols - 1) * gap;
    grid_h = static_cast<float>(rows) * cell + static_cast<float>(rows - 1) * gap;

    const float origin_x = (platform::window::WIDTH - grid_w) * 0.5f;
    const float origin_y = (platform::window::HEIGHT - grid_h) * 0.5f;

    // Generate the table
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const SDL_FRect rect = {
                origin_x + x * (cell + gap),
                origin_y + y * (cell + gap),
                cell,
                cell
            };

            board[y][x].rect = rect;
            board[y][x].bg = platform::game::block::color::BG;
            board[y][x].mines_around = 0;
            board[y][x].is_revealed = false;
            board[y][x].is_flagged = false;
            board[y][x].is_mine = false;
        }
    }

    int mines = board_size.mines;
    amount_of_mines = mines;
    srand(static_cast<unsigned int>(time(nullptr)));

    // Generate Mines
    while (mines > 0) {
        const int x = rand() % cols;
        const int y = rand() % rows;

        if (board[y][x].is_mine != true) {
            board[y][x].is_mine = true;
            --mines;
        }
    }

    // Generate Mine Count Numbers
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (board[y][x].is_mine) continue;

            int count = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (i == 0 && j == 0) continue;

                    if (x + i >= 0 && x + i < cols && y + j >= 0 && y + j < rows && board[y + j][x + i].is_mine) {
                        ++count;
                    }

                    board[y][x].mines_around = count;
                }
            }
        }
    }
}


void Game::generate_grid() const {
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const auto &cell = board[y][x];

            draw_rect(cell.rect, cell.bg);

            if (cell.is_revealed) {
                if (cell.is_mine) {
                    draw_rounded_rect(cell.rect, 20.0f, {0, 0, 0, 255});
                } else {
                    if (cell.mines_around > 0) {
                        const uint8_t radiant = cell.mines_around * 30;
                        const SDL_Color draw_color = {
                            static_cast<uint8_t>(41 + radiant),
                            static_cast<uint8_t>(184 + radiant),
                            static_cast<uint8_t>(255 + radiant),
                            255
                        };

                        const SDL_Rect txt_pos = {
                            static_cast<int>(cell.rect.x + cell.rect.w / 2 - 5),
                            static_cast<int>(cell.rect.y + cell.rect.h / 2 - 7),
                            static_cast<int>(cell.rect.w / 2),
                            static_cast<int>(cell.rect.h / 2)
                        };

                        std::string txt = std::to_string(cell.mines_around);

                        draw_txt(txt_pos,
                                 draw_color,
                                 txt.c_str());
                    }
                }
            }

            if (cell.is_flagged) {
                draw_circle({cell.rect.x + cell.rect.w / 2, cell.rect.y + cell.rect.h / 2, 10}, {255, 0, 0, 255});
            }
        }
    }
}

bool Game::grid_mouse_action(const mouse_pos pos) {
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    if (ignore_left_click_until_release) {
        if (!IS_PRESSED(platform::input::MOUSE_LEFT)) {
            ignore_left_click_until_release = false;
        }
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const auto &cell = board[y][x];

            if (check_hover(cell.rect, pos)) {
                if (!ignore_left_click_until_release &&
                    !cell.is_revealed &&
                    !cell.is_flagged
                    && IS_PRESSED(platform::input::MOUSE_LEFT)) {
                    cell.is_revealed = true;
                    cell.bg = platform::game::block::color::REVELED_BG;

                    if (cell.is_mine) {
                        cell.bg = platform::game::block::color::LOST_BG;
                        return true;
                    }

                    // TODO: Do the same on the first tile unlock
                    loop_around_tile(x, y);
                }
                if (!cell.is_revealed && IS_PRESSED(platform::input::MOUSE_RIGHT)) {
                    cell.is_flagged = !cell.is_flagged;
                }
            } else {
            }
        }
    }

    return false;
}


void Game::loop_around_tile(const int pos_x, const int pos_y) {
    if (board.empty() || board[0].empty()) return;

    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    auto in_bounds = [&](const int x, const int y) {
        return x >= 0 && x < cols && y >= 0 && y < rows;
    };

    if (!in_bounds(pos_x, pos_y)) return;
    if (board[pos_y][pos_x].is_mine) return;

    const bool start_is_zero = (board[pos_y][pos_x].mines_around == 0);
    if (!start_is_zero) {
        return;
    }

    std::queue<std::pair<int, int> > q;

    if (!board[pos_y][pos_x].is_revealed) {
        board[pos_y][pos_x].is_revealed = true;
        board[pos_y][pos_x].bg = platform::game::block::color::REVELED_BG;
    }
    q.emplace(pos_x, pos_y);

    constexpr int directionCount = 8;
    const int dx[directionCount] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[directionCount] = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        for (int i = 0; i < directionCount; ++i) {
            const int nx = cx + dx[i];
            const int ny = cy + dy[i];
            if (!in_bounds(nx, ny)) continue;

            auto &n_cell = board[ny][nx];
            if (n_cell.is_mine || n_cell.is_flagged || n_cell.is_revealed) continue;

            n_cell.is_revealed = true;
            n_cell.bg = platform::game::block::color::REVELED_BG;

            if (n_cell.mines_around == 0) {
                q.emplace(nx, ny);
            }
        }
    }
}


bool Game::check_win() const {
    // TODO: Fix win Check
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    int mines_left = amount_of_mines;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (board[y][x].is_mine && board[y][x].is_revealed) {
                --mines_left;
            }
        }
    }

    return mines_left == 0;
}

