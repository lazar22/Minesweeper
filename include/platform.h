//
// Created by roki on 2025-11-04.
//

#ifndef PLATFORM_H
#define PLATFORM_H

#define READ_KEY(button, key) case(key):{\
input.buttons[button].changed = is_down != input.buttons[button].is_down;\
input.buttons[button].is_down = is_down;\
} break;

namespace platform {
    namespace window {
        static auto TITLE{"Minesweeper"};

        static constexpr int WIDTH{750};
        static constexpr int HEIGHT{600};

        static constexpr SDL_Color COLOR{34, 12, 16, 255};

        namespace score {
            static auto TITLE{"Score Board"};

            constexpr int WIDTH{520};
            constexpr int HEIGHT{360};
        }
    }

    namespace organization {
        const char *const NAME{"Blue Dragon"};
    }

    namespace game_state {
        enum MENU_ACTION {
            TITLE,
            QUIT,
            PLAYING,
            PAUSED,
            SCORE,
        };
    }

    namespace game {
        namespace board {
            typedef struct BOARD_SETTINGS {
                uint8_t w;
                uint8_t h;
                uint8_t mines;
            } board_settings_t;
        }

        namespace block {
            namespace color {
                constexpr SDL_Color BG{148, 148, 148, 255};
                constexpr SDL_Color REVELED_BG{205, 211, 213, 255};
                constexpr SDL_Color LOST_BG{196, 41, 41, 255};
            }

            constexpr int SIZE{50};
            constexpr int OFFSET{2};
        }
    }

    namespace input {
        typedef struct BUTTON_STATE {
            bool is_down;
            bool changed;
        } button_state_t;

        enum BUTTONS {
            UP,
            DOWN,

            W,
            S,
            A,
            D,

            MOUSE_LEFT,
            MOUSE_RIGHT,

            BUTTON_COUNT,
        };

        typedef struct INPUT {
            button_state_t buttons[BUTTON_COUNT];
        } input_t;
    }

    namespace font {
        constexpr int TITLE_SIZE{120};
        constexpr int SUB_TITLE_SIZE{90};

        namespace color {
            constexpr SDL_Color MAIN{119, 203, 185, 255};
            constexpr SDL_Color BG{205, 211, 213, 255};
            constexpr SDL_Color BG_HOVER{80, 108, 100, 255};
        }
    }

    namespace file {
        constexpr char *NAME{"score"};
        constexpr char *TYPE{".json"};
        constexpr int MAX_SCORES_SAVED{10};
    }
}

#endif //PLATFORM_H
