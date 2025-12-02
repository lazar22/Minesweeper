cmake_minimum_required(VERSION 3.14)
project(Minesweeper LANGUAGES CXX)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)

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

target_link_libraries(minesweeper PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2TTF_LIBRARIES}
)

# Copy assets next to the built binary after build
add_custom_command(TARGET minesweeper POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:minesweeper>/assets"
        COMMENT "Copying assets to $<TARGET_FILE_DIR:minesweeper>/assets"
)
