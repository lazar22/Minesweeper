//
// Created by roki on 2025-12-02.
//

#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include <SDL2/SDL.h>
#include <fstream>
#include <string>

#include <platform.h>

class ScoreManager {
    std::string score_file_dir;

public:
    explicit ScoreManager(const std::string _score_file_dir) {
        char *path = SDL_GetPrefPath(platform::organization::NAME, platform::window::TITLE);
        if (!path) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_GetPrefPath failed: %s", SDL_GetError());
            return;
        }

        score_file_dir = path + _score_file_dir + platform::file::FILE_TYPE;
        SDL_free(path);

        auto file = std::fstream(score_file_dir, std::ios::in | std::ios::out);
        if (file.is_open()) {
            std::ofstream create(score_file_dir);

            if (!create) {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create score file: %s", SDL_GetError());
            } else {
                SDL_Log("Successfully created score file: %s", score_file_dir.c_str());
            }
        } else {
            SDL_Log("Using the existing score file.");
        }
    };

    ~ScoreManager() = default;

public:
    void save_score(const float score) const;

    [[nodiscard]] float *load_scores() const;

    void check_score(const float score) const;
};

#endif //SCORE_MANAGER_H
