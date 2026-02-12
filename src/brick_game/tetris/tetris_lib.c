#include "tetris_lib.h"

int del_full_line(Game_intro *val) {
  Game_intro tmp = *val;
  int exp = 0;
  int bonus = 1;

  int s = 19;
  for (int y = 19; y >= 0 && s > 0; y--) {
    int full = 1;
    for (int x = 0; x < 10; x++) {
      val->field[y][x] = tmp.field[s][x];
      if (!val->field[y][x]) {
        full = 0;
      }
    }

    if (full) {
      y++;
      exp += bonus * 100;
      bonus *= 2;
    }
    s--;
  }
  return exp;
}

int endval(Game_intro *val) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (val->fig.field[i][j]) {
        val->field[val->fig.y + i][val->fig.x + j] = 1;
      }
    }
  }

  return del_full_line(val);
}

void rotate(Figure_t *fig) {
  Figure_t tmp = *fig;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      fig->field[i][j] = tmp.field[3 - j][i];
    }
  }
}

int check(Game_intro val) {
  int result = 1;
  for (int i = 0; i < 4 && result; i++) {
    for (int j = 0; j < 4 && result; j++) {
      int is_out_of_bounds =
          val.fig.y + i >= 20 || val.fig.x + j < 0 || val.fig.x + j >= 10;
      int is_collision_with_field = val.field[val.fig.y + i][val.fig.x + j];
      if (val.fig.field[i][j] &&
          (is_collision_with_field || is_out_of_bounds)) {
        result = 0;
      }
    }
  }
  return result;
}

int check_y(Game_intro val) {
  val.fig.y++;
  return !check(val);
}

int check_right(Game_intro val) {
  val.fig.x++;
  return check(val);
}

int check_left(Game_intro val) {
  val.fig.x--;
  return check(val);
}

int check_rotate(Game_intro val) {
  rotate(&val.fig);
  return check(val);
}

void init_figure(Game_intro *val, int num) {
  const int ALL_SHAPES[7][4][4] = {// I
                                   {{0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0}},
                                   // J-фигура
                                   {{0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0}},
                                   // обратный J
                                   {{0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {0}},
                                   // квадрат
                                   {{0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0}},
                                   // гребень
                                   {{0}, {0, 0, 1, 0}, {0, 1, 1, 1}, {0}},
                                   // Z
                                   {{0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0}},
                                   // обратный Z
                                   {{0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0}}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      val->next_fig.field[i][j] = ALL_SHAPES[num][i][j];
    }
  }
  val->next_fig.x = 4;
  val->next_fig.y = -2;
}

long long current_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void move_fig(Game_intro *val, UserAction_t action) {
  int can_move = !val->fall && !val->pause;
  if (action == Pause) {
    val->pause = !val->pause;
  } else if (action == Down && can_move) {
    val->delay_ms = 0;
    val->fall = 1;
  } else if (action == Left && check_left(*val) && can_move) {
    val->fig.x--;
  } else if (action == Right && check_right(*val) && can_move) {
    val->fig.x++;
  } else if (action == Action && check_rotate(*val) && can_move) {
    rotate(&val->fig);
  }

  if (check_y(*val)) {
    val->status = Calc_score;
  }

  long long now = current_millis();
  if (now - val->last_time >= val->delay_ms - (val->level * 20)) {
    val->last_time = now;
    if (!val->pause && val->status == Move_fig) {
      val->fig.y++;
    }
  }
}

void load_high_score(Game_intro *val) {
  FILE *file = fopen("highscore.dat", "rb");
  if (file) {
    size_t read_count = fread(&val->high_score, sizeof(int), 1, file);
    if (read_count != 1) {
      val->high_score = 0;
    }
    fclose(file);
  }
}

void start_init(Game_intro *val) {
  Game_intro null_val = {0};
  *val = null_val;
  val->level = 1;
  init_figure(val, rand() % 7);
  load_high_score(val);
  val->fig.y = 5;
}

void save_high_score(const Game_intro *val) {
  FILE *file = fopen("highscore.dat", "wb");
  if (file) {
    fwrite(&val->score, sizeof(int), 1, file);
    fclose(file);
  }
}

void calc_score(Game_intro *val) {
  val->score += endval(val);
  val->level = val->score / 600;
  val->level = val->level > 10 ? 10 : val->level;
  if (val->score > val->high_score) {
    save_high_score(val);
    val->high_score = val->score;
  }
}

void spawn(Game_intro *val) {
  val->last_time = current_millis();
  val->delay_ms = 400;
  val->fall = 0;
  val->fig = val->next_fig;
  init_figure(val, rand() % 7);
}

Game_intro *core(UserAction_t action) {
  static Game_intro val = {0};

  if (val.status == Start_init && action == Start) {
    start_init(&val);
    val.status = Spawn;
  }
  if (val.status == Spawn) {
    spawn(&val);
    val.status = Move_fig;
  }
  if (val.status == Move_fig) {
    move_fig(&val, action);
  }
  if (val.status == Calc_score) {
    calc_score(&val);
    val.status = val.fig.y < 0 ? Game_over : Spawn;
  }
  if (val.status == Game_over && action == Start) {
    val.status = Start_init;
    start_init(&val);
    val.status = Spawn;
  }

  return &val;
}

void userInput(UserAction_t action, bool hold) {
  core(action);
  if (hold) {
  }
}

GameInfo_t updateCurrentState() {
  Game_intro *tmp = core(Up);
  GameInfo_t result = {0};

  static int game_field_data[20][10];
  static int next_figure_data[4][4];
  static int *field_rows[20];
  static int *next_rows[4];

  static int pointers_initialized = 0;
  if (!pointers_initialized) {
    for (int i = 0; i < 20; i++) {
      field_rows[i] = game_field_data[i];
    }
    for (int i = 0; i < 4; i++) {
      next_rows[i] = next_figure_data[i];
    }
    pointers_initialized = 1;
  }

  result.field = field_rows;
  result.next = next_rows;

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      int is_figure_here = 0;
      if (i >= tmp->fig.y && i < tmp->fig.y + 4 && j >= tmp->fig.x &&
          j < tmp->fig.x + 4) {
        int fig_i = i - tmp->fig.y;
        int fig_j = j - tmp->fig.x;
        if (fig_i >= 0 && fig_i < 4 && fig_j >= 0 && fig_j < 4) {
          is_figure_here = tmp->fig.field[fig_i][fig_j];
        }
      }

      game_field_data[i][j] = tmp->field[i][j] || is_figure_here;
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      next_figure_data[i][j] = tmp->next_fig.field[i][j];
    }
  }

  result.score = tmp->score;
  result.high_score = tmp->high_score;
  result.level = tmp->level;
  result.speed = tmp->level;
  result.pause = tmp->pause;

  return result;
}