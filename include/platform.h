//
// Created by roki on 2025-11-04.
//

#ifndef PLATFORM_H
#define PLATFORM_H

namespace platform {
    namespace window {
        static auto TITLE{"Minesweeper"};

        static constexpr int WIDTH{750};
        static constexpr int HEIGHT{600};

        static constexpr SDL_Color COLOR{204, 204, 204, 255};
    }

    namespace game_state {
        enum {
            TITLE,
            PLAYING,
            PAUSED,
        };
    }

    namespace font {
        constexpr int TITLE_SIZE{120};
        constexpr int SUB_TITLE_SIZE{90};
    }
}

#endif //PLATFORM_H
