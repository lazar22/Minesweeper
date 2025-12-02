//
// Created by roki on 2025-12-02.
//

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>

#include <score_manager.h>
#include <renderer.h>

std::string ScoreManager::open_score_window(TTF_Font *font) {
    SDL_Window *score_window = SDL_CreateWindow(
        platform::window::score::TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        platform::window::score::WIDTH, platform::window::score::HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);

    if (!score_window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create score window: %s", SDL_GetError());
        return "";
    }

    SDL_Renderer *score_renderer = SDL_CreateRenderer(score_window, -1,
                                                      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!score_renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create score renderer: %s", SDL_GetError());
        SDL_DestroyWindow(score_window);
        return "";
    }

    Renderer renderer{score_renderer, font};

    std::string input_name{};
    constexpr size_t max_name_len = 20;
    bool running = true;

    SDL_StartTextInput();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                input_name.clear();
                break;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    input_name.clear();
                    running = false;
                    break;
                }
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    if (!input_name.empty()) input_name.pop_back();
                }
                if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
                    running = false;
                    break;
                }
            }
            if (e.type == SDL_TEXTINPUT) {
                if (input_name.size() < max_name_len) {
                    const char *txt = e.text.text;
                    for (size_t i = 0; txt[i] != '\0'; ++i) {
                        unsigned char c = static_cast<unsigned char>(txt[i]);
                        if (c >= 32 && c != 127) {
                            input_name.push_back(static_cast<char>(c));
                            if (input_name.size() >= max_name_len) break;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(score_renderer, 24, 24, 24, 255);
        SDL_RenderClear(score_renderer);

        const SDL_Rect title_rect{20, 15, platform::window::score::WIDTH - 40, 40};
        renderer.draw_txt(title_rect, {255, 255, 255, 255}, "Leaderboard");

        int y = 60;
        int rank = 1;
        for (const auto &s: score_list) {
            std::ostringstream line;
            line << std::setw(2) << rank << ". " << s.name
                    << "  -  " << std::fixed << std::setprecision(2) << s.time << "s";
            const SDL_Rect line_rect{30, y, platform::window::score::WIDTH - 60, 22};
            renderer.draw_txt(line_rect, {200, 200, 200, 255}, line.str().c_str());
            y += 24;
            if (rank++ >= platform::file::MAX_SCORES_SAVED) break;
        }

        // Input prompt box
        constexpr SDL_FRect box{
            20.0f, static_cast<float>(platform::window::score::HEIGHT - 100),
            static_cast<float>(platform::window::score::WIDTH - 40), 60.0f
        };
        renderer.draw_rounded_rect(box, 8.0f, {48, 48, 48, 255});
        const SDL_Rect prompt_rect{
            static_cast<int>(box.x + 10), static_cast<int>(box.y + 8), static_cast<int>(box.w - 20), 22
        };
        renderer.draw_txt(prompt_rect, {255, 255, 255, 255}, "Enter your name (Enter to save, Esc to cancel):");

        // Input text
        std::string display = input_name.empty() ? std::string("_") : input_name + "_";
        const SDL_Rect input_rect{
            static_cast<int>(box.x + 10), static_cast<int>(box.y + 32), static_cast<int>(box.w - 20), 22
        };
        renderer.draw_txt(input_rect, {180, 220, 180, 255}, display.c_str());

        SDL_RenderPresent(score_renderer);
    }

    SDL_StopTextInput();

    SDL_DestroyRenderer(score_renderer);
    SDL_DestroyWindow(score_window);

    auto trim = [](std::string &s) {
        while (!s.empty() && isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
        while (!s.empty() && isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    };
    trim(input_name);

    return input_name;
}

void ScoreManager::save_score(const std::string &name, const float time) {
    if (score_list.empty()) {
        load_scores();
    }

    score_t new_score{name.empty() ? std::string(platform::window::TITLE) : name, time};

    if (score_list.size() < platform::file::MAX_SCORES_SAVED) {
        score_list.push_back(new_score);
    } else {
        auto worst_score = std::max_element(
            score_list.begin(),
            score_list.end(),
            [](const score_t &a, const score_t &b) { return a.time < b.time; }
        );

        if (time >= worst_score->time) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Time [%.2f] is not in top [%d]",
                        time, platform::file::MAX_SCORES_SAVED);
            return;
        }

        *worst_score = new_score;
    }

    score_list.sort(
        [](const score_t &a, const score_t &b) { return a.time < b.time; });

    if (score_list.size() > platform::file::MAX_SCORES_SAVED) {
        score_list.resize(platform::file::MAX_SCORES_SAVED);
    }

    std::ofstream file(score_file_dir);
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Could not open score file: %s",
                     score_file_dir.c_str());
        return;
    }

    for (const auto &score: score_list) {
        file << score.name << " " << std::fixed << std::setprecision(2) << score.time << std::endl;
    }

    SDL_Log("Saved top %zu scores to %s", score_list.size(), score_file_dir.c_str());
}

std::vector<score_t> ScoreManager::load_scores() {
    std::vector<score_t> scores = {};

    std::ifstream file(score_file_dir);
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Could not open score file: %s",
                     score_file_dir.c_str());
        return scores;
    }

    score_t score;
    while (file >> score.name >> score.time) {
        scores.push_back(score);
        score_list.push_back(score);
    };

    return scores;
}

bool ScoreManager::is_for_the_list(const float time) {
    if (score_list.empty()) {
        load_scores();
    }

    if (score_list.size() < platform::file::MAX_SCORES_SAVED) {
        return true;
    }

    const auto worst_it = std::max_element(
        score_list.begin(), score_list.end(),
        [](const score_t &a, const score_t &b) { return a.time < b.time; }
    );

    if (worst_it == score_list.end()) {
        return true;
    }

    return time < worst_it->time;
}
