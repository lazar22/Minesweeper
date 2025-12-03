cmake_minimum_required(VERSION 3.14)

project(Minesweeper
        LANGUAGES CXX
        VERSION 0.0.1
        DESCRIPTION "Minesweeper clone made in SDL2 and C++ 17"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)

if (UNIX)
    pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
elseif (WIN32)
    find_package(SDL2_ttf CONFIG REQUIRED)
endif ()

add_executable(minesweeper
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/lib/game/game.cpp
        ${CMAKE_SOURCE_DIR}/lib/score_manager/score_manager.cpp
        ${CMAKE_SOURCE_DIR}/lib/renderer/renderer.cpp
        ${CMAKE_SOURCE_DIR}/lib/window/window.cpp
)

target_include_directories(minesweeper PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2TTF_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/game
        ${CMAKE_SOURCE_DIR}/lib/score_manager
        ${CMAKE_SOURCE_DIR}/lib/renderer
        ${CMAKE_SOURCE_DIR}/lib/window
)

if (UNIX)
    target_link_libraries(minesweeper PRIVATE
            ${SDL2_LIBRARIES}
            ${SDL2TTF_LIBRARIES}
    )
elseif (WIN32)
    target_link_libraries(minesweeper PRIVATE
            SDL2::SDL2
            SDL2::SDL2main
            SDL2_ttf::SDL2_ttf
    )
endif ()

# Copy assets next to the built binary after build
add_custom_command(TARGET minesweeper POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:minesweeper>/assets"
        COMMENT "Copying assets to $<TARGET_FILE_DIR:minesweeper>/assets"
)
