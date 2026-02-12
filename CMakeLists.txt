cmake_minimum_required(VERSION 3.16)
project(BrickGame_V2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

#========== БИБЛИОТЕКА SNAKE ==========
set(SOURCES
src/brick_game/snake/snake_game.cpp
    src/brick_game/snake/field.cpp
    src/brick_game/snake/snake.cpp
    src/brick_game/snake/apple.cpp
    src/brick_game/snake/states/idle_state.cpp
    src/brick_game/snake/states/playing_state.cpp
    src/brick_game/snake/states/paused_state.cpp
    src/brick_game/snake/states/game_over_state.cpp
    src/brick_game/snake/snake_interface.cpp
)

add_library(snake_lib STATIC ${SOURCES})

target_include_directories(snake_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brick_game/snake/states
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brick_game/snake
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brick_game
)

if(Release)

    # ========== БИБЛИОТЕКА TETRIS ==========
    add_library(tetris_lib STATIC
        src/brick_game/tetris/tetris_lib.c
    )

    target_include_directories(tetris_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/brick_game/tetris
        ${CMAKE_CURRENT_SOURCE_DIR}/src/brick_game
    )

    # ========== CLI ПРИЛОЖЕНИЯ ==========
    find_package(Curses REQUIRED)

    add_executable(tetris_cli
        src/gui/cli/brick_game_cli.c
    )
    target_link_libraries(tetris_cli tetris_lib ${CURSES_LIBRARIES})

    add_executable(snake_cli
        src/gui/cli/brick_game_cli.c
        src/brick_game/snake/snake_interface.cpp
    )
    target_link_libraries(snake_cli snake_lib ${CURSES_LIBRARIES})

    # ========== Desktop ПРИЛОЖЕНИЯ ==========
    set(CMAKE_AUTOMOC ON)
    find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

    add_executable(tetris_desktop
        src/gui/desktop/mainwindow.cpp
    )
    target_link_libraries(tetris_desktop Qt6::Core Qt6::Widgets tetris_lib)

    add_executable(snake_desktop
        src/gui/desktop/mainwindow.cpp
    )
    target_link_libraries(snake_desktop Qt6::Core Qt6::Widgets snake_lib)
endif()

# ================ ТЕСТЫ ========================
option(BUILD_TESTING "Build tests" OFF)

if(BUILD_TESTING)

    enable_testing()

    find_package(GTest REQUIRED)

    add_executable(snake_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/test_snake_game.cpp
    )

    target_include_directories(snake_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )

    target_link_libraries(snake_tests
        GTest::gtest
        GTest::gtest_main
        snake_lib
    )

    add_test(NAME snake_tests COMMAND snake_tests)
    add_test(NAME GameOverStateTest COMMAND game_over_state_test)
    add_test(NAME PlayingStateTest COMMAND playing_state_test)
    add_test(NAME SnakeGameTest COMMAND snake_game_test)

endif()

# ================ Документация ========================
option(BUILD_DVI "Build docs" OFF)

if(BUILD_DVI)

    find_package(Doxygen)

    if (DOXYGEN_FOUND)
        # Настройки Doxygen
        set(DOXYGEN_PROJECT_NAME "Snake Game")
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_LATEX NO)
        set(DOXYGEN_EXTRACT_ALL YES) # Документировать всё, даже без комментов
        set(DOXYGEN_OUTPUT_LANGUAGE Russian)
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_HAVE_DOT YES)
        set(JAVADOC_AUTOBRIEF YES)
        set(SOURCE_BROWSER YES)
        set(QUIET = YES)
        
        # Создаем команду 'doxygen'
        doxygen_add_docs(doxygen 
            ${CMAKE_CURRENT_SOURCE_DIR}/src 
            ALL 
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
        )
    endif()
endif()