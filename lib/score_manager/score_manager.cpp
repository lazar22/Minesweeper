//
// Created by roki on 2025-12-02.
//

#include <algorithm>
#include <iomanip>
#include <score_manager.h>

void ScoreManager::save_score(const float time) {
    if (score_list.empty()) {
        load_scores();
    }

    score_t new_score{platform::window::TITLE, time};

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
