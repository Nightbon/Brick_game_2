#ifndef TETRIS_LIB_H
#define TETRIS_LIB_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "../common.h"

/**
 * @brief Структура для описания фигуры в игре (4x4 поле и координаты x,y).
 */
typedef struct {
  int field[4][4];  ///< Клетки фигуры (1 - занята, 0 - пусто)
  int x;            ///< Позиция фигуры по горизонтали
  int y;            ///< Позиция фигуры по вертикали
} Figure_t;

/**
 * @brief Статусы игрового состояния.
 */
typedef enum {
  Start_init,  ///< Инициализация игры
  Spawn,       ///< Появление новой фигуры
  Move_fig,    ///< Фигура движется
  Calc_score,  ///< Расчет очков
  Game_over    ///< Игра окончена
} Status_t;

/**
 * @brief Основная структура состояния игры, включая игровое поле, текущую и
 * следующую фигуру, счет и прочее.
 */
typedef struct {
  int field[20][10];  ///< Игровое поле (20 строк на 10 колонок)
  Figure_t fig;       ///< Текущая фигура
  Figure_t next_fig;  ///< Следующая фигура
  int high_score;     ///< Рекорд
  int score;          ///< Текущий счет
  int level;          ///< Уровень
  int pause;          ///< Флаг паузы (1 - пауза, 0 - нет)
  int delay_ms;  ///< Задержка между ходами в миллисекундах
  int fall;  ///< Флаг падения фигуры
  long long last_time;  ///< Метка времени последнего обновления
  int status;  ///< Текущий статус игры (Status_t)
} Game_intro;

/**
 * @brief Удаляет полностью заполненные линии из игрового поля и возвращает
 * начисленные очки.
 * @param val Указатель на текущее состояние игры.
 * @return Начисленные очки за удаленные линии.
 */
int del_full_line(Game_intro *val);

/**
 * @brief Обновляет поле после установки фигуры и возвращает очки за завершение
 * линий.
 * @param val Указатель на текущее состояние игры.
 * @return Начисленные очки.
 */
int endval(Game_intro *val);

/**
 * @brief Проверяет, можно ли разместить фигуру в текущей позиции.
 * @param val Состояние игры.
 * @return 1, если фигура размещена корректно, 0 в противном случае.
 */
int check(Game_intro val);

/**
 * @brief Проверяет, возможно ли сместить фигуру вниз.
 * @param val Состояние игры.
 * @return 1, если возможно, 0 если столкновение.
 */
int check_y(Game_intro val);

/**
 * @brief Проверяет, возможно ли сместить фигуру вправо.
 * @param val Состояние игры.
 * @return 1, если возможно, 0 если столкновение.
 */
int check_right(Game_intro val);

/**
 * @brief Проверяет, возможно ли сместить фигуру влево.
 * @param val Состояние игры.
 * @return 1, если возможно, 0 если столкновение.
 */
int check_left(Game_intro val);

/**
 * @brief Проверяет, возможно ли повернуть фигуру.
 * @param val Состояние игры.
 * @return 1, если поворот возможен, 0 если столкновение.
 */
int check_rotate(Game_intro val);

/**
 * @brief Инициализирует фигуру с заданным номером формы.
 * @param val Указатель на состояние игры.
 * @param num Номер фигуры (от 0 до 6).
 */
void init_figure(Game_intro *val, int num);

/**
 * @brief Выполняет действие пользователя (перемещение, пауза, поворот и т.д.).
 * @param val Указатель на состояние игры.
 * @param action Действие пользователя.
 */
void move_fig(Game_intro *val, UserAction_t action);

/**
 * @brief Загружает рекорд из файла.
 * @param val Указатель на состояние игры.
 */
void load_high_score(Game_intro *val);

/**
 * @brief Поворачивает фигуру на 90 градусов.
 * @param fig Указатель на фигуру.
 */
void rotate(Figure_t *fig);

/**
 * @brief Инициализирует игровое состояние.
 * @param val Указатель на состояние игры.
 */
void start_init(Game_intro *val);

/**
 * @brief Сохраняет рекорд в файл.
 * @param val Указатель на состояние игры.
 */
void save_high_score(const Game_intro *val);

/**
 * @brief Рассчитывает счет после завершения линии.
 * @param val Указатель на состояние игры.
 */
void calc_score(Game_intro *val);

/**
 * @brief Спавнит (создает) новую фигуру.
 * @param val Указатель на состояние игры.
 */
void spawn(Game_intro *val);

/**
 * @brief Возвращает текущее время в миллисекундах.
 * @return Время в миллисекундах.
 */
long long current_millis();

/**
 * @brief Основная игровая функция, обрабатывающая состояние на основе действия
 * пользователя.
 * @param action Действие пользователя.
 * @return Текущее состояние игры.
 */
Game_intro *core(UserAction_t action);

/**
 * \mainpage Игра Тетрис
 * \section A Конечный автомат игры
 *
 * Диаграмма состояний тетриса:
 * \dot
 * digraph TetrisStates {
    rankdir=UD;
    node [shape=rectangle, style=filled, fillcolor=lightblue, fontsize=8];
    edge [arrowhead=normal, fontsize=7];

    // Состояния
    Start_init [label="Start_init\n(Инициализация игры)", fillcolor=lightgreen];
    Spawn [label="Spawn\n(Спавн фигуры)", fillcolor=yellow];
    Move_fig [label="Move_fig\n(Движение фигуры)", fillcolor=orange];
    Calc_score [label="Calc_score\n(Расчёт очков)", fillcolor=lightcoral];
    Game_over [label="Game_over\n(Игра окончена)", fillcolor=red];

    // Переходы
    Start_init -> Spawn [label="Start\n(Нажатие Enter/Start)"];
    Spawn -> Move_fig;
    Move_fig -> Calc_score [label="фигура установлена"];
    Calc_score -> Spawn [label="фигура ниже верхней границы"];
    Calc_score -> Game_over [label="фигура выше верхней границы"];
    Game_over -> Start_init;
 * }
 * \enddot
 */

#endif
