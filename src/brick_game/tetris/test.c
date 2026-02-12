#include <check.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tetris_lib.h"

START_TEST(test_core) {
  Game_info_t result = {0};
  //////////////////////////////////////////
  result = get_game_info();
  ck_assert_int_eq(result.status, Start_init);
  ///////////////////////////////////////////
  user_input(Start);
  result = get_game_info();
  ck_assert_int_eq(result.status, Move_fig);
  //////////////////////////////////////////
  user_input(Pause);
  user_input(Pause);
  result = get_game_info();
  ck_assert_int_eq(result.status, Move_fig);
  ///////////////////////////////////////////
  user_input(Left);
  result = get_game_info();
  ck_assert_int_eq(result.status, Move_fig);
  ///////////////////////////////////////////
  user_input(Right);
  result = get_game_info();
  ck_assert_int_eq(result.status, Move_fig);
  ///////////////////////////////////////////
  for (int i = 0; i < 300; i++) {
    user_input(Down);
  }
  user_input(Start);
  result = get_game_info();
  ck_assert_int_eq(result.status, Spawn);
}
END_TEST

START_TEST(test_del_full_line) {
  Game_intro val = {0};
  for (int i = 0; i < 10; i++) {
    val.field[18][i] = 1;
  }
  for (int i = 0; i < 10; i++) {
    val.field[19][i] = 1;
  }
  ck_assert_int_eq(del_full_line(&val), 300);
}
END_TEST

START_TEST(test_move_fig) {
  Game_intro val = {0};
  Game_intro res = {0};
  init_figure(&val, 3);
  init_figure(&res, 3);
  val.fig.y = 10;
  move_fig(&val, Action);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(val.fig.field[i][j], res.fig.field[i][j]);
    }
  }
}
END_TEST

START_TEST(test_calc_score) {
  Game_intro val = {0};
  val.score = 100;
  calc_score(&val);
  ck_assert_int_eq(val.hi_score, 100);
}
END_TEST

START_TEST(test_load_high_score) {
  Game_intro val = {0};
  val.score = 555;
  save_high_score(&val);
  load_high_score(&val);
  ck_assert_int_eq(val.hi_score, 555);
}
END_TEST

TCase *tcase_tetris(void) {
  TCase *tc = tcase_create("Tetris Functions");
  tcase_add_test(tc, test_core);
  tcase_add_test(tc, test_del_full_line);
  tcase_add_test(tc, test_move_fig);
  tcase_add_test(tc, test_calc_score);
  tcase_add_test(tc, test_load_high_score);
  return tc;
}

Suite *tetris_suite(void) {
  Suite *s = suite_create("Tetris Library Tests");
  suite_add_tcase(s, tcase_tetris());
  return s;
}

int main() {
  SRunner *sr = srunner_create(tetris_suite());
  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_VERBOSE);

  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failed == 0) ? 0 : 1;
}