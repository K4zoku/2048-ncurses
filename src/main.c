#define _XOPEN_SOURCE_EXTENDED

#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "utils/common.h"
#include "commons/gamesettings.h"
#include "stages/menu.h"
#include "stages/gameplay.h"
#include "stages/settings.h"

static void init();

static void end();

static stage_fn_var(stages[]) = {
    NULL, // 0 is exit, so this slot is not available
    *stageMenu,
    *stageGameplay,
    *stageMenu, // there is no help
    *stageSettings
};

static int n_stages = sizeof(stages) / sizeof(stage_fn_t);
static GameSettings *settings = DEFAULT_SETTINGS;

int main() {
  int stage = 1;
  init();
  while (inRangeX(stage, 0, n_stages)) 
    stage_invoke(stage, settings);
  end();
  return 0;
}

static void init() {
  setlocale(LC_CTYPE, "");
  srand(time(0));
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  use_default_colors();
  init_pair(1, -1, COLOR_RED);
  init_pair(2, -1, COLOR_GREEN);
  init_pair(3, -1, COLOR_YELLOW);
  init_pair(4, -1, COLOR_BLUE);
  init_pair(5, -1, COLOR_MAGENTA);
  init_pair(6, -1, COLOR_CYAN);

  init_pair(7, -1, -1);
  init_pair(8, COLOR_WHITE, COLOR_BLACK);
}

static void end() {
  endwin();
}
