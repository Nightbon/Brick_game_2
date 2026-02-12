#include "brick_game_cli.h"

int main() {
  setlocale(LC_ALL, "");
  srand(time(NULL));
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  WINDOW *game_win = newwin(22, 24, 0, 0);
  WINDOW *info_win = newwin(22, 24, 0, 24);

  UserAction_t action;
  while ((action = read_input()) != Terminate) {
    GameInfo_t tetris = updateCurrentState();
    printfield(game_win, tetris);
    printinfo(info_win, tetris);
    napms(50);
  }

  clear_win(game_win, info_win);
  return 0;
}

void printfield(WINDOW *game_win, GameInfo_t tetris) {
  werase(game_win);
  box(game_win, 0, 0);

  for (int i = 0; i < 20 && tetris.field; i++) {
    for (int j = 0; j < 10 && tetris.field[i]; j++) {
      if (tetris.field[i][j]) {
        mvwprintw(game_win, i + 1, (j + 1) * 2, "[]");
      }
    }
  }
  wrefresh(game_win);
}

void printinfo(WINDOW *info_win, GameInfo_t tetris) {
  werase(info_win);
  box(info_win, 0, 0);

  for (int i = 0; i < 4 && tetris.next; i++) {
    for (int j = 0; j < 4 && tetris.next[i]; j++) {
      if (tetris.next[i][j]) {
        mvwprintw(info_win, 9 + i, 7 + j * 2, "[]");
      }
    }
  }
  mvwprintw(info_win, 1, 1, "HI-Score: %d", tetris.high_score);
  mvwprintw(info_win, 3, 1, "Score: %d", tetris.score);
  mvwprintw(info_win, 5, 1, "level: %d", tetris.level);
  mvwprintw(info_win, 7, 1, "Speed: %d", tetris.speed);
  if (tetris.pause) {
    mvwprintw(info_win, 13, 9, "PAUSE");
  }
  mvwprintw(info_win, 18, 3, "PRESS \'s\' TO START");
  mvwprintw(info_win, 20, 1, "p - Pause; q - Quit");
  wrefresh(info_win);
}

void clear_win(WINDOW *game_win, WINDOW *info_win) {
  delwin(game_win);
  delwin(info_win);
  endwin();
}

UserAction_t read_input() {
  int ch = getch();
  flushinp();
  UserAction_t action = Up;
  if (ch == KEY_DOWN) {
    action = Down;
    userInput(action, false);
  } else if (ch == KEY_LEFT) {
    action = Left;
    userInput(action, false);
  } else if (ch == KEY_RIGHT) {
    action = Right;
    userInput(action, false);
  } else if (ch == KEY_UP) {
    action = Up;
    userInput(action, false);
  } else if (ch == ' ') {
    action = Action;
    userInput(action, false);
  } else if (ch == 'q') {
    action = Terminate;
    userInput(action, false);
  } else if (ch == 'p') {
    action = Pause;
    userInput(action, false);
  } else if (ch == 's') {
    action = Start;
    userInput(action, false);
  }
  return action;
}