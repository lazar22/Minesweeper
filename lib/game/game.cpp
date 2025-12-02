//
// Created by roki on 2025-11-04.
//

#include "game.h"

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <algorithm>
#include <string>
#include <limits>
#include <memory>
#include <queue>

static SDL_Color current_start_color = platform::font::color::BG;
static SDL_Color current_quit_color = platform::font::color::BG;

static SDL_FRect current_start_rect;
static SDL_FRect current_quit_rect;

static float grid_w;
static float grid_h;

static std::vector<std::vector<block_stats_t> > board;

static int amount_of_mines{0};

static bool ignore_left_click_until_release{false};
static bool init_generation{false};
static bool is_gen{false};

platform::game_state::MENU_ACTION Game::start_menu(SDL_Window *window, const mouse_pos pos) const {
    constexpr int btn_h = {100};
    constexpr int btn_w = {200};
    constexpr int btn_offset = {20};

    constexpr int border_r = {10};

    constexpr int middle_x = (platform::window::WIDTH - btn_w) / 2;
    constexpr int middle_y = (platform::window::HEIGHT - btn_h) / 2;

    constexpr SDL_FRect start_btn = {middle_x, middle_y, btn_w, btn_h};
    constexpr SDL_FRect quit_btn = {middle_x, middle_y + btn_offset + btn_h, btn_w, btn_h};

    renderer_utils->draw_rounded_rect(start_btn, border_r, current_start_color);
    renderer_utils->draw_rounded_rect(quit_btn, border_r, current_quit_color);

    SDL_SetWindowTitle(window, platform::window::TITLE);

    renderer_utils->draw_txt({
                                 static_cast<int>(start_btn.x), static_cast<int>(start_btn.y),
                                 static_cast<int>(start_btn.w), static_cast<int>(start_btn.h)
                             },
                             platform::font::color::MAIN,
                             "Start");

    renderer_utils->draw_txt({
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

    if (is_gen) {
        const std::string title = std::string(platform::window::TITLE) + " " + temp;
        SDL_SetWindowTitle(window, title.c_str());
    }

    // If true, you lost the game
    if (grid_mouse_action(pos)) {
        // TODO: Add a wait before going to the title screen or have a more user friendly output that you lost the game
        return platform::game_state::TITLE;
    }

    // If true, you won the game
    if (check_win()) {
        // TODO: check if the time is in the best of 10, and if it is return to the platform::game_state::SCORE
        SDL_Log("You won the game!");
        if (score_manager->is_for_the_list(elapsed_time)) {
            // Open leaderboard/name entry window on top of the game
            const std::string name = score_manager->open_score_window(font);
            score_manager->save_score(name, static_cast<float>(elapsed_time));
        }
        return platform::game_state::TITLE;
    }

    return platform::game_state::PLAYING;
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

    amount_of_mines = board_size.mines;
    is_gen = false;
}


void Game::generate_tiles(const int safe_x, const int safe_y) const {
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    int mines = amount_of_mines;
    srand(static_cast<unsigned int>(time(nullptr)));

    while (mines > 0) {
        const int x = rand() % cols;
        const int y = rand() % rows;

        // TODO: Make sure the surrounding area [3 x 3] dose not have mines as well
        if (!board[y][x].is_mine && !(x == safe_x && y == safe_y)) {
            board[y][x].is_mine = true;
            --mines;
        }
    }

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

            renderer_utils->draw_rect(cell.rect, cell.bg);

            if (cell.is_revealed) {
                if (cell.is_mine) {
                    renderer_utils->draw_rounded_rect(cell.rect, 20.0f, {0, 0, 0, 255});
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

                        renderer_utils->draw_txt(txt_pos,
                                                 draw_color,
                                                 txt.c_str());
                    }
                }
            }

            if (cell.is_flagged) {
                renderer_utils->draw_circle({cell.rect.x + cell.rect.w / 2, cell.rect.y + cell.rect.h / 2, 10},
                                            {255, 0, 0, 255});
            }
        }
    }
}

bool Game::grid_mouse_action(const mouse_pos pos) const {
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

                    if (!is_gen) {
                        generate_tiles(x, y);
                        is_gen = true;
                    }

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


void Game::loop_around_tile(const int pos_x, const int pos_y) const {
    if (board.empty() || board[0].empty()) return;

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
    if (board.empty() || board[0].empty()) return false;

    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const auto &cell = board[y][x];
            if (!cell.is_mine && !cell.is_revealed) {
                return false;
            }
        }
    }

    return true;
}

bool Game::in_bounds(const int x, const int y) const {
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());

    return x >= 0 && x < cols && y >= 0 && y < rows;
}

// Render
void Game::set_bg_color(const SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}
