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

        static constexpr SDL_Color COLOR{34, 12, 16, 255};
    }

    namespace game_state {
        enum {
            TITLE,
            PLAYING,
            PAUSED,
        };
    }

    namespace input {
        enum BUTTONS {
            UP,
            DOWN,

            W,
            S,
            A,
            D,

            BUTTON_COUNT,
        };
    }

    namespace font {
        constexpr int TITLE_SIZE{120};
        constexpr int SUB_TITLE_SIZE{90};

        namespace color {
            constexpr SDL_Color MAIN{119, 203, 185, 255};
            constexpr SDL_Color BG{205, 211, 213, 255};
        }
    }
}

#endif //PLATFORM_H
