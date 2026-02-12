#ifndef BRICK_GAME_CLI_H
#define BRICK_GAME_CLI_H

#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "../../brick_game/tetris/tetris_lib.h"

#ifdef __cplusplus
#include "../../brick_game/snake/snake_controller.h"
#endif

/**
 * @brief Отрисовывает игровое поле в указанном окне.
 *
 * @param game_win Указатель на окно ncurses для игрового поля.
 * @param val Текущие данные игры для отображения.
 */
void printfield(WINDOW *game_win, GameInfo_t val);

/**
 * @brief Отрисовывает игровую информацию (счет, уровень и т.д.) в указанном
 * окне.
 *
 * @param info_win Указатель на окно ncurses для информации.
 * @param val Текущие данные игры для отображения.
 */
void printinfo(WINDOW *info_win, GameInfo_t val);

/**
 * @brief Освобождает ресурсы, связанные с окнами и завершает режим ncurses.
 *
 * @param game_win Указатель на окно игрового поля.
 * @param info_win Указатель на окно с информацией.
 */
void clear_win(WINDOW *game_win, WINDOW *info_win);

/**
 * @brief Считывает и интерпретирует ввод пользователя, возвращая действие.
 *
 * @return Действие пользователя в виде UserAction_t.
 */
UserAction_t read_input();

#endif  // BRICK_GAME_CLI_H
