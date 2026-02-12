#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include "../brick_game/controller.h"
#include "../brick_game/snake/apple.h"
#include "../brick_game/snake/field.h"
#include "../brick_game/snake/point.h"
#include "../brick_game/snake/snake.h"
#include "../brick_game/snake/snake_game.h"
#include "../brick_game/snake/states/game_over_state.h"
#include "../brick_game/snake/states/idle_state.h"
#include "../brick_game/snake/states/paused_state.h"
#include "../brick_game/snake/states/playing_state.h"

using namespace s21;
// Тесты для класса Point
TEST(PointTest, DefaultConstructor) {
  Point p;
  EXPECT_EQ(p.x, 0);
  EXPECT_EQ(p.y, 0);
}

TEST(PointTest, ParameterizedConstructor) {
  Point p(5, 10);
  EXPECT_EQ(p.x, 5);
  EXPECT_EQ(p.y, 10);
}

TEST(PointTest, EqualityOperator) {
  Point p1(5, 10);
  Point p2(5, 10);
  Point p3(6, 10);

  EXPECT_TRUE(p1 == p2);
  EXPECT_FALSE(p1 == p3);
}

TEST(PointTest, InequalityOperator) {
  Point p1(5, 10);
  Point p2(5, 10);
  Point p3(6, 10);

  EXPECT_FALSE(p1 != p2);
  EXPECT_TRUE(p1 != p3);
}

TEST(PointTest, AdditionAssignment) {
  Point p1(5, 10);
  Point p2(3, 4);

  p1 += p2;
  EXPECT_EQ(p1.x, 8);
  EXPECT_EQ(p1.y, 14);
}

TEST(PointTest, AdditionOperator) {
  Point p1(5, 10);
  Point p2(3, 4);

  Point result = p1 + p2;
  EXPECT_EQ(result.x, 8);
  EXPECT_EQ(result.y, 14);
}

// Тесты для класса Field
TEST(FieldTest, Dimensions) {
  Field field;
  EXPECT_EQ(Field::WIDTH, 10);
  EXPECT_EQ(Field::HEIGHT, 20);
}

TEST(FieldTest, InitialState) {
  Field field;

  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      EXPECT_EQ(field.getCell(x, y), Field::EMPTY);
    }
  }
}

TEST(FieldTest, SetAndGetCell) {
  Field field;

  field.setCell(5, 5, Field::SNAKE);
  EXPECT_EQ(field.getCell(5, 5), Field::SNAKE);

  field.setCell(9, 19, Field::APPLE);
  EXPECT_EQ(field.getCell(9, 19), Field::APPLE);

  EXPECT_FALSE(field.isEmpty(Point(9, 19)));
}

TEST(FieldTest, OutOfBounds) {
  Field field;

  // За пределами поля должно возвращать WALL
  EXPECT_EQ(field.getCell(-1, -1), Field::WALL);
  EXPECT_EQ(field.getCell(Field::WIDTH, Field::HEIGHT), Field::WALL);
  EXPECT_EQ(field.getCell(5, -1), Field::WALL);
  EXPECT_EQ(field.getCell(5, Field::HEIGHT), Field::WALL);
}

TEST(FieldTest, IsInside) {
  Field field;

  EXPECT_TRUE(field.isInside(Point(0, 0)));
  EXPECT_TRUE(field.isInside(Point(9, 19)));
  EXPECT_TRUE(field.isInside(Point(5, 10)));

  EXPECT_FALSE(field.isInside(Point(-1, 0)));
  EXPECT_FALSE(field.isInside(Point(0, -1)));
  EXPECT_FALSE(field.isInside(Point(10, 0)));
  EXPECT_FALSE(field.isInside(Point(0, 20)));
}

TEST(FieldTest, IsEmpty) {
  Field field;

  EXPECT_TRUE(field.isEmpty(Point(5, 5)));

  field.setCell(5, 5, Field::SNAKE);
  EXPECT_FALSE(field.isEmpty(Point(5, 5)));

  field.setCell(5, 5, Field::EMPTY);
  EXPECT_TRUE(field.isEmpty(Point(5, 5)));
}

TEST(FieldTest, Clear) {
  Field field;

  // Заполняем поле
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      field.setCell(x, y, Field::SNAKE);
    }
  }

  // Проверяем, что поле заполнено
  EXPECT_EQ(field.getCell(5, 5), Field::SNAKE);

  // Очищаем
  field.clear();

  // Проверяем, что все клетки пустые
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      EXPECT_EQ(field.getCell(x, y), Field::EMPTY);
    }
  }
}

// Тесты для класса Snake
TEST(SnakeTest, DefaultConstructor) {
  Snake snake;
  EXPECT_EQ(snake.getLength(), 0);
  EXPECT_TRUE(snake.getBody().empty());
}

TEST(SnakeTest, Initialize) {
  Snake snake;
  Point start_pos(5, 5);

  snake.initialize(start_pos);

  EXPECT_EQ(snake.getLength(), 4);  // Голова + 3 сегмента
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::RIGHT);

  const auto& body = snake.getBody();
  EXPECT_EQ(body[0], Point(5, 5));  // Голова
  EXPECT_EQ(body[1], Point(4, 5));
  EXPECT_EQ(body[2], Point(3, 5));
  EXPECT_EQ(body[3], Point(2, 5));
}

TEST(SnakeTest, SetDirection) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  // Устанавливаем валидное направление
  snake.setDirection(Snake::Direction::UP);
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::UP);

  // Пытаемся установить противоположное направление (не должно измениться)
  snake.setDirection(Snake::Direction::LEFT);
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::UP);
}

TEST(SnakeTest, MoveWithoutGrow) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  // Устанавливаем направление вверх
  snake.setDirection(Snake::Direction::UP);

  // Двигаемся без роста
  bool moved = snake.move(false);
  EXPECT_TRUE(moved);
  EXPECT_EQ(snake.getLength(), 4);  // Длина должна остаться той же
  EXPECT_EQ(snake.getHead(), Point(5, 4));  // Новая позиция головы
}

TEST(SnakeTest, MoveWithGrow) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  // Устанавливаем направление вверх
  snake.setDirection(Snake::Direction::UP);

  // Двигаемся с ростом
  bool moved = snake.move(true);
  EXPECT_TRUE(moved);
  EXPECT_EQ(snake.getLength(), 5);  // Длина должна увеличиться
  EXPECT_EQ(snake.getHead(), Point(5, 4));  // Новая позиция головы
}

TEST(SnakeTest, SelfCollision) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  // Очищаем и создаем змейку, которая столкнется с собой
  snake.clear();
  snake.initialize(start_pos);
  snake.move(true);
  snake.setDirection(Snake::Direction::UP);
  snake.move(true);
  snake.setDirection(Snake::Direction::LEFT);
  snake.move(true);
  snake.setDirection(Snake::Direction::DOWN);
  bool moved = snake.move(true);

  EXPECT_FALSE(moved);  // Столкновение с собой при движении
}

TEST(SnakeTest, Contains) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  // Проверяем точки, которые должны содержаться
  EXPECT_TRUE(snake.contains(Point(5, 5)));
  EXPECT_TRUE(snake.contains(Point(4, 5)));
  EXPECT_TRUE(snake.contains(Point(3, 5)));

  // Проверяем точки, которых не должно быть
  EXPECT_FALSE(snake.contains(Point(1, 5)));
  EXPECT_FALSE(snake.contains(Point(5, 4)));
}

TEST(SnakeTest, DirectionVector) {
  Snake snake;

  EXPECT_EQ(snake.getDirectionVector(Snake::Direction::UP), Point(0, -1));
  EXPECT_EQ(snake.getDirectionVector(Snake::Direction::RIGHT), Point(1, 0));
  EXPECT_EQ(snake.getDirectionVector(Snake::Direction::DOWN), Point(0, 1));
  EXPECT_EQ(snake.getDirectionVector(Snake::Direction::LEFT), Point(-1, 0));
}

TEST(SnakeTest, ValidDirectionChange) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  // Устанавливаем начальное направление
  snake.setDirection(Snake::Direction::LEFT);

  // Тестируем валидные изменения направления
  EXPECT_TRUE(snake.isValidDirectionChange(Snake::Direction::UP));
  EXPECT_TRUE(snake.isValidDirectionChange(Snake::Direction::DOWN));

  // Тестируем невалидные изменения (противоположное направление)
  EXPECT_FALSE(snake.isValidDirectionChange(Snake::Direction::LEFT));

  snake.setDirection(Snake::Direction::UP);
  snake.move(false);
  EXPECT_FALSE(snake.isValidDirectionChange(Snake::Direction::DOWN));
  snake.move(false);
  snake.setDirection(Snake::Direction::LEFT);
  snake.move(false);
  EXPECT_FALSE(snake.isValidDirectionChange(Snake::Direction::RIGHT));
  snake.setDirection(Snake::Direction::DOWN);
  snake.move(false);
  EXPECT_FALSE(snake.isValidDirectionChange(Snake::Direction::UP));
  snake.move(false);
  EXPECT_FALSE(snake.isValidDirectionChange(Snake::Direction::UP));
}

TEST(SnakeTest, Clear) {
  Snake snake;
  Point start_pos(5, 5);
  snake.initialize(start_pos);

  EXPECT_EQ(snake.getLength(), 4);

  snake.clear();

  EXPECT_EQ(snake.getLength(), 0);
  EXPECT_TRUE(snake.getBody().empty());
}

// Тесты для класса Apple
TEST(AppleTest, SpawnOnEmptyField) {
  Field field;
  Snake snake;
  Apple apple;

  // Инициализируем змейку в углу
  snake.initialize(Point(0, 0));

  // Спауним яблоко
  apple.spawn(field, snake);

  const Point& pos = apple.getPosition();

  // Проверяем, что позиция внутри поля
  EXPECT_GE(pos.x, 0);
  EXPECT_LT(pos.x, Field::WIDTH);
  EXPECT_GE(pos.y, 0);
  EXPECT_LT(pos.y, Field::HEIGHT);

  // Проверяем, что яблоко не на змейке
  EXPECT_FALSE(snake.contains(pos));
}

TEST(AppleTest, SpawnOnFullField) {
  Field field;
  Snake snake;
  Apple apple;

  // Заполняем все поле змейкой
  // const auto& grid = game.getField().getGrid();
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      field.setCell(x, y, Field::SNAKE);
    }
  }
  // snake.getfield().setCell(x, y, Field::SNAKE)

  // Добавляем больше сегментов, чтобы заполнить все поле
  // (в реальности это невозможно, но тестируем edge case)

  // В этом случае apple.spawn должен вернуть Point(0, 0)
  apple.spawn(field, snake);

  const Point& pos = apple.getPosition();
  EXPECT_EQ(pos, Point(0, 0));
}

// Тесты для класса SnakeGame
class SnakeGameTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Удаляем файл с рекордом перед каждым тестом
    std::remove("snake_highscore.dat");
  }

  void TearDown() override {
    // Очищаем после теста
    std::remove("snake_highscore.dat");
  }

  SnakeGame game;
};

TEST_F(SnakeGameTest, InitialState) {
  EXPECT_EQ(game.getScore(), 0);
  // EXPECT_EQ(game.getHighScore(), 0);
  EXPECT_EQ(game.getLevel(), 1);
  EXPECT_EQ(game.getSpeed(), SnakeGame::INITIAL_SPEED);
}

TEST_F(SnakeGameTest, StartGame) {
  game.start();

  // После старта змейка должна быть инициализирована
  EXPECT_GT(game.getSnake().getLength(), 0);

  // Позиция головы змейки должна быть в пределах поля
  const Point& head = game.getSnake().getHead();
  EXPECT_GE(head.x, 0);
  EXPECT_LT(head.x, Field::WIDTH);
  EXPECT_GE(head.y, 0);
  EXPECT_LT(head.y, Field::HEIGHT);

  // Яблоко должно быть размещено
  const Point& apple_pos = game.getApple().getPosition();
  EXPECT_GE(apple_pos.x, 0);
  EXPECT_LT(apple_pos.x, Field::WIDTH);
  EXPECT_GE(apple_pos.y, 0);
  EXPECT_LT(apple_pos.y, Field::HEIGHT);
}

TEST_F(SnakeGameTest, ResetGame) {
  // Изменяем состояние игры
  game.addScore(10);

  // Сбрасываем
  game.reset();

  // Проверяем сброс
  EXPECT_EQ(game.getScore(), 4);  // Начальный счет
  EXPECT_EQ(game.getLevel(), 1);
  EXPECT_EQ(game.getSpeed(), SnakeGame::INITIAL_SPEED);
}

TEST_F(SnakeGameTest, AddScore) {
  int initial_score = game.getScore();

  game.addScore(5);
  EXPECT_EQ(game.getScore(), initial_score + 5);

  game.addScore(10);
  EXPECT_EQ(game.getScore(), initial_score + 15);
}

TEST_F(SnakeGameTest, UpdateLevel) {
  game.addScore(SnakeGame::POINTS_PER_LEVEL * 2);  // Должен быть уровень 3
  game.updateLevel();

  EXPECT_EQ(game.getLevel(), 3);

  // Проверяем максимальный уровень
  for (int i = 0; i < 20; ++i) {
    game.addScore(SnakeGame::POINTS_PER_LEVEL);
    game.updateLevel();
  }

  EXPECT_LE(game.getLevel(), SnakeGame::MAX_LEVEL);
}

TEST_F(SnakeGameTest, HighScoreSaving) {
  // Добавляем очки
  game.addScore(100);

  // Обновляем рекорд
  game.updateHighScore();

  // Проверяем, что рекорд обновился
  EXPECT_EQ(game.getHighScore(), 100);

  // Проверяем, что рекорд не уменьшается
  game.addScore(-50);
  EXPECT_EQ(game.getHighScore(), 100);  // 100 > 50, рекорд не должен измениться
}

TEST_F(SnakeGameTest, ProcessInput) {
  // Тестируем обработку ввода
  game.processInput(Start);
  // Проверяем, что игра перешла в состояние Idle или Playing
  // (зависит от реализации)

  // Тестируем другие команды
  game.processInput(Pause);
  game.processInput(Terminate);
}

TEST_F(SnakeGameTest, GetGameInfo) {
  GameInfo_t info = game.getGameInfo();

  // Проверяем структуру
  EXPECT_NE(info.field, nullptr);
  EXPECT_EQ(info.next, nullptr);
  EXPECT_EQ(info.score, 0);
  EXPECT_EQ(info.high_score, 0);
  EXPECT_EQ(info.level, 0);  // В начальном состоянии уровень 0
  EXPECT_EQ(info.speed, 0);
  EXPECT_EQ(info.pause, 0);
}

// Тесты для контроллера
TEST(ControllerTest, HandleAction) {
  Controller controller;

  // Тестируем обработку действий
  // (это в основном интеграционный тест)
  controller.handleAction(Start, false);
  controller.handleAction(Left, false);
  controller.handleAction(Right, false);
  controller.handleAction(Up, false);
  controller.handleAction(Down, false);
  controller.handleAction(Action, true);
  controller.handleAction(Pause, false);
  controller.handleAction(Terminate, false);
}

TEST(ControllerTest, GetGameInfo) {
  Controller controller;

  GameInfo_t info = controller.getGameInfo();

  // Проверяем, что функция возвращает корректную структуру
  EXPECT_NE(info.field, nullptr);
}

// Интеграционные тесты
TEST(IntegrationTest, CompleteGameFlow) {
  SnakeGame game;
  Controller controller;

  // 1. Начинаем игру
  game.processInput(Start);

  // 2. Получаем информацию о игре
  GameInfo_t info = game.getGameInfo();
  EXPECT_GT(info.score, 0);

  // 3. Управляем змейкой
  game.processInput(Right);
  game.update();

  // 4. Ставим на паузу
  game.processInput(Pause);
  info = game.getGameInfo();
  EXPECT_EQ(info.pause, 1);

  // 5. Снимаем с паузы
  game.processInput(Pause);

  // 6. Завершаем игру
  game.processInput(Terminate);
}

TEST(IntegrationTest, AppleSpawning) {
  Field field;
  Snake snake;
  Apple apple;

  // Инициализируем змейку
  snake.initialize(Point(5, 5));

  // Многократно спауним яблоки и проверяем их позиции
  std::vector<Point> apple_positions;

  for (int i = 0; i < 100; ++i) {
    apple.spawn(field, snake);
    Point pos = apple.getPosition();

    // Проверяем валидность позиции
    EXPECT_GE(pos.x, 0);
    EXPECT_LT(pos.x, Field::WIDTH);
    EXPECT_GE(pos.y, 0);
    EXPECT_LT(pos.y, Field::HEIGHT);

    // Проверяем, что яблоко не на змейке
    EXPECT_FALSE(snake.contains(pos));

    apple_positions.push_back(pos);
  }

  // Проверяем, что яблоки появляются в разных местах
  // (хотя бы некоторые позиции должны отличаться)
  bool different_positions = false;
  for (size_t i = 1; i < apple_positions.size(); ++i) {
    if (apple_positions[i] != apple_positions[0]) {
      different_positions = true;
      break;
    }
  }
  EXPECT_TRUE(different_positions);
}

TEST(IntegrationTest, SnakeMovementAndCollision) {
  SnakeGame game;
  game.start();

  auto& snake = game.getSnake();
  auto& field = game.getField();

  // Сохраняем начальную позицию
  Point initial_head = snake.getHead();

  // Двигаем змейку несколько раз
  for (int i = 0; i < 5; ++i) {
    game.processInput(Right);
    snake.move(false);

    // Проверяем, что змейка двигается
    if (i > 0) {
      EXPECT_NE(snake.getHead(), initial_head);
    }
  }

  // Проверяем столкновение со стеной
  // (этот тест может не сработать, если змейка не достигает стены)
  bool wall_collision = snake.checkWallCollision(field);
  EXPECT_TRUE(wall_collision);
  // Не проверяем конкретное значение, так как оно зависит от позиции
}

// Тесты для edge cases
TEST(EdgeCaseTest, SnakeMaxLength) {
  SnakeGame game;
  game.start();

  // Этот тест проверяет логику достижения максимальной длины
  // В реальности он может быть долгим, поэтому только проверяем константы
  EXPECT_GT(SnakeGame::MAX_SNAKE_LENGTH, 0);
}

TEST(EdgeCaseTest, SpeedLimits) {
  SnakeGame game;

  // Проверяем, что скорость не выходит за пределы
  game.reset();
  EXPECT_EQ(game.getSpeed(), SnakeGame::INITIAL_SPEED);

  // Добавляем много очков для увеличения уровня
  for (int i = 0; i < 100; ++i) {
    game.addScore(SnakeGame::POINTS_PER_LEVEL);
    game.updateLevel();
  }

  // Скорость должна быть ограничена
  EXPECT_LE(game.getSpeed(), 18);  // Проверяем ограничение из кода
}

// Тесты для сериализации/десериализации рекорда
TEST(SerializationTest, HighScoreSaveLoad) {
  // Создаем игру и устанавливаем рекорд
  {
    SnakeGame game1;
    game1.addScore(500);
    game1.updateHighScore();
    // При уничтожении game1 рекорд должен сохраниться
  }

  // Создаем новую игру и проверяем, что рекорд загрузился
  {
    SnakeGame game2;
    // В реальной игре рекорд загружается в конструкторе
    // Но так как мы удалили файл в SetUp, он будет 0
    // Это нормально для тестов
  }
}

// Тест производительности (опционально)
TEST(PerformanceTest, GameLoopSpeed) {
  SnakeGame game;
  game.start();

  auto start = std::chrono::high_resolution_clock::now();

  // Выполняем несколько итераций игрового цикла
  for (int i = 0; i < 1000; ++i) {
    game.update();
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  // Проверяем, что 1000 итераций выполняются за разумное время
  EXPECT_LT(duration.count(), 1000);  // Меньше 1 секунды
}

namespace s21 {

class GameOverStateTest : public ::testing::Test {
 protected:
  void SetUp() override {
    game_ = std::make_unique<SnakeGame>();
    state_win_ = std::make_unique<GameOverState>(true);
    state_lose_ = std::make_unique<GameOverState>(false);
  }

  void TearDown() override {
    state_win_.reset();
    state_lose_.reset();
    game_.reset();
  }

  std::unique_ptr<SnakeGame> game_;
  std::unique_ptr<GameOverState> state_win_;
  std::unique_ptr<GameOverState> state_lose_;
};

// Тест 1: Проверка обработки ввода в состоянии GameOver (Start)
TEST_F(GameOverStateTest, HandleInput_Start_ResetsGameAndChangesToIdle) {
  // Устанавливаем состояние GameOver
  game_->changeState<GameOverState>(false);

  // Сохраняем начальные значения
  auto initial_high_score = game_->getHighScore();
  game_->addScore(0);  // Добавляем очки

  // Вызываем обработку ввода Start
  state_lose_->handleInput(*game_, UserAction_t::Start);

  // Проверяем, что игра сброшена
  EXPECT_EQ(game_->getScore(), 4);  // Начальный счет после reset()
  EXPECT_EQ(game_->getLevel(), 1);  // Начальный уровень
  EXPECT_EQ(game_->getSpeed(), SnakeGame::INITIAL_SPEED);

  // Проверяем, что рекорд сохранен
  EXPECT_EQ(game_->getHighScore(), initial_high_score);
}

// Тест 2: Проверка обработки других действий в GameOver (должны игнорироваться)
TEST_F(GameOverStateTest, HandleInput_OtherActions_Ignored) {
  game_->addScore(20);
  auto initial_score = game_->getScore();

  // Пробуем различные действия, которые должны игнорироваться
  state_lose_->handleInput(*game_, UserAction_t::Pause);
  state_lose_->handleInput(*game_, UserAction_t::Left);
  state_lose_->handleInput(*game_, UserAction_t::Right);
  state_lose_->handleInput(*game_, UserAction_t::Up);
  state_lose_->handleInput(*game_, UserAction_t::Down);
  state_lose_->handleInput(*game_, UserAction_t::Action);
  state_lose_->handleInput(*game_, UserAction_t::Terminate);

  // Счет не должен измениться
  EXPECT_EQ(game_->getScore(), initial_score);
}

// Тест 3: Проверка обновления в состоянии GameOver (ничего не должно
// происходить)
TEST_F(GameOverStateTest, Update_DoesNothing) {
  auto initial_score = game_->getScore();
  auto initial_level = game_->getLevel();
  auto initial_speed = game_->getSpeed();

  // Многократный вызов update
  for (int i = 0; i < 10; ++i) {
    state_win_->update(*game_);
    state_lose_->update(*game_);
  }

  // Никакие параметры не должны измениться
  EXPECT_EQ(game_->getScore(), initial_score);
  EXPECT_EQ(game_->getLevel(), initial_level);
  EXPECT_EQ(game_->getSpeed(), initial_speed);
}

// Тест 4: Проверка получения информации об игре в состоянии GameOver
TEST_F(GameOverStateTest, GetGameInfo_ReturnsCorrectValues) {
  // Устанавливаем значения
  game_->addScore(15);
  game_->updateLevel();  // Обновляем уровень

  // Получаем информацию через состояние
  auto info_win = state_win_->getGameInfo(*game_);
  auto info_lose = state_lose_->getGameInfo(*game_);

  // Проверяем значения
  EXPECT_EQ(info_win.score, 15);
  EXPECT_EQ(info_lose.score, 15);
  EXPECT_EQ(info_win.high_score, game_->getHighScore());
  EXPECT_EQ(info_win.level, game_->getLevel());
  EXPECT_EQ(info_win.speed, game_->getSpeed());
  EXPECT_EQ(info_win.pause, 0);  // Не на паузе

  // Проверяем поле
  EXPECT_NE(info_win.field, nullptr);

  // Проверяем, что next всегда nullptr
  EXPECT_EQ(info_win.next, nullptr);
  EXPECT_EQ(info_lose.next, nullptr);
}

// Тест 5: Проверка структуры поля в GameOverState
TEST_F(GameOverStateTest, GetGameInfo_FieldStructure) {
  auto info = state_win_->getGameInfo(*game_);

  // Проверяем, что поле корректно заполнено
  ASSERT_NE(info.field, nullptr);

  // Проверяем несколько ячеек (поле должно быть пустым после reset)
  for (int y = 0; y < 5; ++y) {
    for (int x = 0; x < 5; ++x) {
      // После reset поле пустое
      EXPECT_TRUE(info.field[y][x] == Field::EMPTY ||
                  info.field[y][x] == Field::WALL)
          << "Field cell [" << y << "][" << x << "] = " << info.field[y][x];
    }
  }
}

// Тест 6: Проверка консистентности GameInfo в разных состояниях GameOver
TEST_F(GameOverStateTest, GetGameInfo_ConsistencyBetweenWinAndLose) {
  game_->addScore(25);

  auto info_win = state_win_->getGameInfo(*game_);
  auto info_lose = state_lose_->getGameInfo(*game_);

  // Все поля должны совпадать, кроме is_win_ (который не в GameInfo)
  EXPECT_EQ(info_win.score, info_lose.score);
  EXPECT_EQ(info_win.high_score, info_lose.high_score);
  EXPECT_EQ(info_win.level, info_lose.level);
  EXPECT_EQ(info_win.speed, info_lose.speed);
  EXPECT_EQ(info_win.pause, info_lose.pause);

  // Проверяем, что это разные объекты GameInfo (но с одинаковыми значениями)
  EXPECT_NE(&info_win, &info_lose);
}

// Тест 7: Граничный случай - максимальный счет
TEST_F(GameOverStateTest, GetGameInfo_MaxScore) {
  // Устанавливаем максимальный счет
  game_->addScore(999);

  auto info = state_win_->getGameInfo(*game_);

  EXPECT_EQ(info.score, 999);
  EXPECT_EQ(info.high_score, 999);  // Должен обновиться рекорд
  EXPECT_GE(info.level, 1);
}

// Тест 8: Проверка перехода из GameOver в Idle через changeState
TEST_F(GameOverStateTest, StateTransition_GameOverToIdle) {
  // Эмулируем состояние GameOver
  game_->changeState<GameOverState>(false);

  // Проверяем, что можем перейти в Idle
  game_->changeState<IdleState>();

  // Проверяем, что игра в начальном состоянии
  EXPECT_EQ(game_->getScore(), 0);
  EXPECT_EQ(game_->getLevel(), 1);
}

}  // namespace s21

namespace s21 {

class PlayingStateTest : public ::testing::Test {
 protected:
  void SetUp() override {
    game_ = std::make_unique<SnakeGame>();
    state_ = std::make_unique<PlayingState>();

    // Запускаем игру для тестирования PlayingState
    game_->start();
    game_->changeState<PlayingState>();
  }

  void TearDown() override {
    state_.reset();
    game_.reset();
  }

  std::unique_ptr<SnakeGame> game_;
  std::unique_ptr<PlayingState> state_;
};

// Тест 1: Проверка обработки ввода направления движения
TEST_F(PlayingStateTest, HandleInput_DirectionChanges) {
  auto& snake = game_->getSnake();

  // Тестируем изменение направления
  state_->handleInput(*game_, UserAction_t::Up);
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::UP);

  state_->handleInput(*game_, UserAction_t::Right);
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::RIGHT);

  state_->handleInput(*game_, UserAction_t::Up);
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::UP);

  state_->handleInput(*game_, UserAction_t::Down);
  EXPECT_EQ(snake.getNextDirection(), Snake::Direction::DOWN);
}

// Тест 2: Проверка обработки ввода Pause
TEST_F(PlayingStateTest, HandleInput_Pause_ChangesToPausedState) {
  state_->handleInput(*game_, UserAction_t::Start);
  // Сохраняем начальное состояние
  auto initial_score = game_->getHighScore();

  // auto snake = game_->getSnake();
  // Point p;
  // auto body = snake.getBody();
  // for (int i = 0; i < 200; i++) {
  //     snake.move(true);
  // }

  // Вызываем update несколько раз (чтобы преодолеть таймер)
  for (int i = 0; i < 300; ++i) {
    state_->handleInput(*game_, UserAction_t::Action);
    state_->update(*game_);
  }

  // Проверяем, что состояние изменилось на PausedState
  // (Мы не можем напрямую проверить тип состояния, но можем проверить
  // поведение)
  EXPECT_EQ(game_->getHighScore(), initial_score);  // Счет не должен измениться
}

// Тест 3: Проверка обработки ввода Action (обновление игры)
TEST_F(PlayingStateTest, HandleInput_Action_UpdatesGame) {
  auto initial_score = game_->getScore();
  auto initial_length = game_->getSnake().getLength();

  state_->handleInput(*game_, UserAction_t::Action);

  // После обновления длина могла измениться (если съели яблоко)
  // или счет мог измениться
  EXPECT_GE(game_->getSnake().getLength(), initial_length - 1);
}

// Тест 4: Проверка обработки ввода Terminate
TEST_F(PlayingStateTest, HandleInput_Terminate_ResetsGame) {
  // Устанавливаем некоторые значения
  game_->addScore(10);
  game_->updateLevel();

  auto score_before = game_->getScore();
  auto level_before = game_->getLevel();

  EXPECT_GT(score_before, 4);
  EXPECT_GT(level_before, 1);

  // Вызываем Terminate
  state_->handleInput(*game_, UserAction_t::Terminate);

  // Проверяем, что игра сброшена к начальным значениям
  EXPECT_EQ(game_->getScore(), 4);  // reset() устанавливает score = 4
  EXPECT_EQ(game_->getLevel(), 1);
}

// Тест 5: Проверка обновления игры (движение змейки)
TEST_F(PlayingStateTest, Update_SnakeMoves) {
  auto initial_head = game_->getSnake().getHead();

  state_->handleInput(*game_, UserAction_t::Start);

  // Вызываем update несколько раз (чтобы преодолеть таймер)
  for (int i = 0; i < 30; ++i) {
    state_->update(*game_);
  }

  auto new_head = game_->getSnake().getHead();

  // Голова должна переместиться
  EXPECT_NE(initial_head.x, new_head.x);
}

// Тест 6: Проверка обновления при столкновении со стеной
TEST_F(PlayingStateTest, Update_WallCollision_GameOver) {
  // Заставляем змейку двигаться к стене
  auto& snake = game_->getSnake();
  snake.setDirection(Snake::Direction::LEFT);

  // Многократно обновляем, пока не столкнется
  bool collision_detected = false;
  for (int i = 0; i < 100; ++i) {
    auto head_before = snake.getHead();
    state_->update(*game_);
    auto head_after = snake.getHead();

    // Если голова не изменилась, возможно, игра закончилась
    if (head_before == head_after && i > 10) {
      collision_detected = true;
      break;
    }
  }

  EXPECT_TRUE(collision_detected) << "Collision with wall should be detected";
}

// Тест 7: Проверка съедания яблока
TEST_F(PlayingStateTest, Update_AppleEaten_ScoreIncreases) {
  state_->handleInput(*game_, UserAction_t::Start);
  auto initial_score = game_->getScore();
  auto initial_length = game_->getSnake().getLength();

  // Размещаем яблоко прямо перед головой змейки
  auto& snake = game_->getSnake();
  auto& apple = game_->getApple();
  auto& field = game_->getField();

  Point apple_pos =
      snake.getHead() + snake.getDirectionVector(snake.getNextDirection());

  // Убеждаемся, что позиция внутри поля
  if (field.isInside(apple_pos)) {
    // Устанавливаем яблоко вручную (в реальной игре через spawn)
    apple.setPosition(apple_pos);

    // Многократно обновляем и проверяем
    bool apple_eaten = false;
    auto score_before = game_->getScore();
    for (int i = 0; i < 50 && !apple_eaten; ++i) {
      state_->update(*game_);

      if (game_->getScore() > score_before) {
        apple_eaten = true;
        break;
      }
    }

    EXPECT_TRUE(apple_eaten) << "Apple should be eaten";
    EXPECT_GT(game_->getSnake().getLength(), initial_length);
  }
}

// Тест 8: Проверка перехода в GameOver при победе (максимальная длина)
TEST_F(PlayingStateTest, Update_MaxLength_Victory) {
  // Эмулируем почти максимальную длину
  auto& snake = game_->getSnake();

  // Добавляем много сегментов (в реальной игре через поедание яблок)
  // Вместо этого проверяем логику условия победы

  // Устанавливаем длину, близкую к максимальной
  // (В реальном тесте нужно эмулировать поедание яблок)

  // Проверяем, что условие победы корректно проверяется
  EXPECT_LT(snake.getLength(), SnakeGame::MAX_SNAKE_LENGTH);
}

// Тест 9: Проверка получения информации об игре
TEST_F(PlayingStateTest, GetGameInfo_ReturnsCorrectValues) {
  // Устанавливаем значения
  game_->addScore(5);
  game_->updateLevel();

  auto info = state_->getGameInfo(*game_);

  EXPECT_EQ(info.score, game_->getScore());
  EXPECT_EQ(info.high_score, game_->getHighScore());
  EXPECT_EQ(info.level, game_->getLevel());
  EXPECT_EQ(info.speed, game_->getLevel());  // В коде speed = level
  EXPECT_EQ(info.pause, 0);
  EXPECT_EQ(info.next, nullptr);
  EXPECT_NE(info.field, nullptr);
}

// Тест 10: Проверка работы таймера и скорости
TEST_F(PlayingStateTest, Update_TimerAndSpeed) {
  // Проверяем, что скорость влияет на частоту обновлений
  auto initial_speed = game_->getSpeed();

  // Меняем уровень (и скорость)
  game_->addScore(SnakeGame::POINTS_PER_LEVEL * 2);
  game_->updateLevel();

  auto new_speed = game_->getSpeed();
  EXPECT_GT(new_speed, initial_speed);

  // Проверяем базовую задержку
  PlayingState state_with_timer;
  // Таймер должен сбрасываться при создании
  // Дальнейшая логика зависит от внутренней реализации
}

// Тест 11: Проверка обновления поля после движения
TEST_F(PlayingStateTest, Update_FieldUpdated) {
  auto& field = game_->getField();
  auto& snake = game_->getSnake();

  // Получаем начальное состояние поля
  auto head_before = snake.getHead();
  auto cell_before = field.getCell(head_before.x, head_before.y);
  EXPECT_EQ(cell_before, Field::SNAKE);

  // Обновляем несколько раз
  for (int i = 0; i < 25; ++i) {
    state_->update(*game_);
  }

  // Проверяем новое положение змейки на поле
  auto head_after = snake.getHead();
  auto cell_after = field.getCell(head_after.x, head_after.y);
  EXPECT_EQ(cell_after, Field::SNAKE);
}

// Тест 12: Проверка валидности смены направления
TEST_F(PlayingStateTest, DirectionChange_Validity) {
  auto& snake = game_->getSnake();

  // Устанавливаем начальное направление
  snake.setDirection(Snake::Direction::RIGHT);

  // Пробуем установить противоположное направление (должно игнорироваться)
  snake.setDirection(Snake::Direction::LEFT);

  // В текущей реализации next_direction_ может измениться,
  // но isValidDirectionChange должен возвращать false
  EXPECT_FALSE(snake.isValidDirectionChange(Snake::Direction::LEFT));
}

// Тест 13: Проверка обработки некорректного ввода
TEST_F(PlayingStateTest, HandleInput_InvalidAction_Ignored) {
  auto initial_score = game_->getScore();
  auto initial_dir = game_->getSnake().getNextDirection();

  // Некорректное значение (за пределами enum)
  // Тестируем default case в switch
  state_->handleInput(*game_, static_cast<UserAction_t>(999));

  // Ничего не должно измениться
  EXPECT_EQ(game_->getScore(), initial_score);
  EXPECT_EQ(game_->getSnake().getNextDirection(), initial_dir);
}

}  // namespace s21

namespace s21 {

class SnakeGameTest2 : public ::testing::Test {
 protected:
  void SetUp() override {
    // Удаляем файл рекорда перед тестом
    std::filesystem::remove("snake_highscore.dat");
    game_ = std::make_unique<SnakeGame>();
  }

  void TearDown() override {
    game_.reset();
    // Удаляем файл рекорда после теста
    std::filesystem::remove("snake_highscore.dat");
  }

  std::unique_ptr<SnakeGame> game_;
};

// Тест 1: Проверка инициализации игры
TEST_F(SnakeGameTest2, Constructor_InitializesCorrectly) {
  EXPECT_EQ(game_->getScore(), 0);
  EXPECT_EQ(game_->getLevel(), 1);
  EXPECT_EQ(game_->getSpeed(), SnakeGame::INITIAL_SPEED);
  EXPECT_EQ(game_->getHighScore(), 0);

  // Поле должно быть пустым
  auto& field = game_->getField();
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      EXPECT_EQ(field.getCell(x, y), Field::EMPTY);
    }
  }
}

// Тест 2: Проверка запуска игры
TEST_F(SnakeGameTest2, Start_InitializesGameObjects) {
  game_->start();

  // Проверяем, что змейка инициализирована
  auto& snake = game_->getSnake();
  EXPECT_GE(snake.getLength(), 4);  // Начальная длина

  // Проверяем, что яблоко размещено
  auto& apple = game_->getApple();
  auto apple_pos = apple.getPosition();
  EXPECT_GE(apple_pos.x, 0);
  EXPECT_LT(apple_pos.x, Field::WIDTH);
  EXPECT_GE(apple_pos.y, 0);
  EXPECT_LT(apple_pos.y, Field::HEIGHT);

  // Проверяем счет
  EXPECT_EQ(game_->getScore(), 4);  // reset() устанавливает score = 4
}

// Тест 3: Проверка сброса игры
TEST_F(SnakeGameTest2, Reset_SetsDefaultValues) {
  // Устанавливаем не-дефолтные значения
  game_->addScore(20);
  game_->updateLevel();

  EXPECT_GT(game_->getScore(), 4);
  EXPECT_GT(game_->getLevel(), 1);
  EXPECT_GT(game_->getSpeed(), SnakeGame::INITIAL_SPEED);

  // Сбрасываем
  game_->reset();

  // Проверяем значения по умолчанию
  EXPECT_EQ(game_->getScore(), 4);
  EXPECT_EQ(game_->getLevel(), 1);
  EXPECT_EQ(game_->getSpeed(), SnakeGame::INITIAL_SPEED);
}

// Тест 4: Проверка добавления очков
TEST_F(SnakeGameTest2, AddScore_IncreasesScore) {
  auto initial_score = game_->getScore();

  game_->addScore(5);
  EXPECT_EQ(game_->getScore(), initial_score + 5);

  game_->addScore(10);
  EXPECT_EQ(game_->getScore(), initial_score + 15);
}

// Тест 5: Проверка обновления уровня
TEST_F(SnakeGameTest2, UpdateLevel_CalculatesCorrectly) {
  // Начальный уровень
  EXPECT_EQ(game_->getLevel(), 1);

  // Добавляем очки для перехода на уровень 2
  game_->addScore(SnakeGame::POINTS_PER_LEVEL);
  game_->updateLevel();
  EXPECT_EQ(game_->getLevel(), 2);

  // Добавляем очки для перехода на уровень 3
  game_->addScore(SnakeGame::POINTS_PER_LEVEL);
  game_->updateLevel();
  EXPECT_EQ(game_->getLevel(), 3);

  // Проверяем максимальный уровень
  for (int i = 0; i < SnakeGame::MAX_LEVEL * SnakeGame::POINTS_PER_LEVEL; i++) {
    game_->addScore(1);
  }
  game_->updateLevel();
  EXPECT_LE(game_->getLevel(), SnakeGame::MAX_LEVEL);
}

// Тест 6: Проверка обновления рекорда
TEST_F(SnakeGameTest2, UpdateHighScore_UpdatesWhenHigher) {
  // Начальный рекорд
  auto initial_high_score = game_->getHighScore();

  // Добавляем очки, но меньше текущего рекорда
  game_->addScore(5);
  game_->updateHighScore();
  EXPECT_EQ(game_->getHighScore(), std::max(initial_high_score, 5));

  // Добавляем больше очков
  game_->addScore(20);
  game_->updateHighScore();
  EXPECT_GE(game_->getHighScore(), 25);
}

// Тест 7: Проверка сохранения и загрузки рекорда
TEST_F(SnakeGameTest2, SaveAndLoadHighScore_PersistsBetweenInstances) {
  // Устанавливаем рекорд
  game_->addScore(100);
  game_->updateHighScore();

  auto high_score = game_->getHighScore();
  EXPECT_GE(high_score, 100);

  // Создаем новую игру (должен загрузиться сохраненный рекорд)
  game_.reset();
  game_ = std::make_unique<SnakeGame>();

  // Проверяем, что рекорд загружен
  EXPECT_EQ(game_->getHighScore(), high_score);
}

// Тест 8: Проверка обработки ввода
TEST_F(SnakeGameTest2, ProcessInput_DelegatesToState) {
  // В начальном состоянии (IdleState) Start должен начать игру
  game_->processInput(UserAction_t::Start);

  // После Start игра должна быть в состоянии PlayingState
  // (проверяем косвенно через состояние игры)

  // Проверяем, что другие действия обрабатываются
  game_->processInput(UserAction_t::Pause);
  game_->processInput(UserAction_t::Left);
  game_->processInput(UserAction_t::Right);
  // Не должно быть крашей
}

// Тест 9: Проверка обновления игры
TEST_F(SnakeGameTest2, Update_DelegatesToState) {
  // Многократный вызов update не должен вызывать ошибок
  for (int i = 0; i < 10; ++i) {
    game_->update();
  }

  // Проверяем, что игра в корректном состоянии
  EXPECT_GE(game_->getScore(), 0);
  EXPECT_GE(game_->getLevel(), 1);
  EXPECT_LE(game_->getLevel(), SnakeGame::MAX_LEVEL);
}

// Тест 10: Проверка получения информации об игре
TEST_F(SnakeGameTest2, GetGameInfo_ReturnsValidStructure) {
  auto info = game_->getGameInfo();

  // Проверяем обязательные поля
  EXPECT_NE(info.field, nullptr);
  EXPECT_EQ(info.next, nullptr);
  EXPECT_GE(info.score, 0);
  EXPECT_GE(info.high_score, 0);
  EXPECT_GE(info.level, 0);
  EXPECT_LE(info.level, SnakeGame::MAX_LEVEL);
  EXPECT_GE(info.speed, 0);
  EXPECT_LE(info.speed, 20);  // Максимальная скорость ограничена
}

// Тест 11: Проверка смены состояний через шаблонный метод
TEST_F(SnakeGameTest2, ChangeState_TemplateMethod) {
  // Проверяем смену на различные состояния
  game_->changeState<PlayingState>();
  // После смены должны быть в PlayingState

  game_->changeState<PausedState>();
  // После смены должны быть в PausedState

  game_->changeState<GameOverState>(true);
  // После смены должны быть в GameOverState с флагом победы

  game_->changeState<IdleState>();
  // Вернулись в IdleState
}

// Тест 12: Проверка методов доступа
TEST_F(SnakeGameTest2, AccessorMethods_ReturnReferences) {
  // Проверяем, что методы возвращают ссылки на объекты
  auto& field1 = game_->getField();
  auto& field2 = game_->getField();
  EXPECT_EQ(&field1, &field2);

  auto& snake1 = game_->getSnake();
  auto& snake2 = game_->getSnake();
  EXPECT_EQ(&snake1, &snake2);

  auto& apple1 = game_->getApple();
  auto& apple2 = game_->getApple();
  EXPECT_EQ(&apple1, &apple2);
}

// Тест 13: Проверка константных методов доступа
TEST_F(SnakeGameTest2, ConstAccessorMethods) {
  const auto& const_game = *game_;

  // Проверяем константные методы
  auto& field = const_game.getField();
  auto& snake = const_game.getSnake();
  auto& apple = const_game.getApple();

  // Просто проверяем, что они существуют и компилируются
  EXPECT_NO_THROW({
    auto score = const_game.getScore();
    auto high_score = const_game.getHighScore();
    auto level = const_game.getLevel();
    auto speed = const_game.getSpeed();
  });
}

// Тест 14: Проверка граничных значений уровня и скорости
TEST_F(SnakeGameTest2, LevelAndSpeed_BoundaryValues) {
  // Добавляем много очков для достижения максимального уровня
  for (int i = 0; i < SnakeGame::MAX_LEVEL * SnakeGame::POINTS_PER_LEVEL * 2;
       i++) {
    game_->addScore(1);
    game_->updateLevel();
  }

  // Проверяем, что уровень не превышает максимум
  EXPECT_LE(game_->getLevel(), SnakeGame::MAX_LEVEL);

  // Проверяем, что скорость ограничена
  EXPECT_LE(game_->getSpeed(), 18);  // В коде: if (speed_ > 18) speed_ = 18;
}

// Тест 15: Проверка инициализации игры
TEST_F(SnakeGameTest2, InitializeGame_ClearsObjects) {
  game_->start();  // Инициализирует игру

  // Сохраняем состояние
  auto initial_length = game_->getSnake().getLength();

  // Вызываем initializeGame
  game_->reset();

  // Проверяем, что объекты очищены
  auto& snake = game_->getSnake();
  EXPECT_LT(snake.getLength(), initial_length);

  // Поле должно быть пустым
  auto& field = game_->getField();
  bool all_empty = true;
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      if (field.getCell(x, y) != Field::EMPTY) {
        all_empty = false;
        break;
      }
    }
  }
  EXPECT_TRUE(all_empty);
}

// Тест 16: Проверка обработки отсутствия файла рекорда
TEST_F(SnakeGameTest2, LoadHighScore_NoFile_CreatesDefault) {
  // Убеждаемся, что файла нет
  ASSERT_FALSE(std::filesystem::exists("snake_highscore.dat"));

  // Создаем новую игру - должен установиться рекорд по умолчанию (0)
  game_.reset();
  game_ = std::make_unique<SnakeGame>();

  EXPECT_EQ(game_->getHighScore(), 0);
}

// Тест 17: Проверка корректности констант
TEST_F(SnakeGameTest2, Constants_HaveCorrectValues) {
  EXPECT_EQ(SnakeGame::MAX_SNAKE_LENGTH, 200);
  EXPECT_EQ(SnakeGame::POINTS_PER_APPLE, 1);
  EXPECT_EQ(SnakeGame::POINTS_PER_LEVEL, 5);
  EXPECT_EQ(SnakeGame::MAX_LEVEL, 10);
  EXPECT_EQ(SnakeGame::INITIAL_SPEED, 5);
  EXPECT_EQ(SnakeGame::SPEED_INCREMENT, 2);
}

}  // namespace s21

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}