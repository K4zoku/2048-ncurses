#define _XOPEN_SOURCE_EXTENDED

#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include "utils/common.h"
#include "stages/settings.h"

#define WIDTH 32
#define HEIGHT 8

static void reset();

static void display(const int selected);

static void select(const int selected);

static void unselect(const int selected);

static const char *entries[] = {
    "Board size",
    "Win condition",
    "Tile spacing",
    "Back",
};

static const int n_entries = sizeof(entries) / sizeof(char *);

static WINDOW *win = NULL;
static int selected = 0;

void stageSettings(int *stage, GameSettings *settings) {
  int l;
  for (;;) {
    reset();
    display(selected);
    l = numlen(settings->gridSize);
    mvwprintw(win, 2, 28 - l - 2, "[ %d ]", settings->gridSize);
    l = numlen(settings->winCondition);
    mvwprintw(win, 3, 28 - l - 2, "[ %d ]", settings->winCondition);
    l = settings->tileSpacing ? numlen(settings->tileSpacing) : 1;
    mvwprintw(win, 4, 28 - l - 2, "[ %d ]", settings->tileSpacing);
    wrefresh(win);
    switch (wgetch(win)) {
      case 0x1b:
        selected = 3;
        goto back;
      case 'w':
      case KEY_UP:
        if (--selected < 0) selected = n_entries - 1;
        break;
      case 's':
      case KEY_DOWN:
        if (++selected == n_entries) selected = 0;
        break;
      case '\n':
      case KEY_ENTER:
      case KEY_RIGHT:
        switch (selected) {
          case 0:
            if (settings->gridSize <= 6) ++settings->gridSize;
            break;
          case 1:
            if (settings->winCondition < 0xFFFF) settings->winCondition <<= 1;
            break;
          case 2:
            if (settings->tileSpacing < 3) ++settings->tileSpacing;
            break;
          case 3:
            goto back;
        }
        break;
      case KEY_LEFT:
        switch (selected) {
          case 0:
            if (settings->gridSize > 2) --settings->gridSize;
            break;
          case 1:
            if (settings->winCondition > 8) settings->winCondition >>= 1;
            break;
          case 2:
            if (settings->tileSpacing > 0) --settings->tileSpacing;
            break;
          case 3:
            goto back;
        }
        break;
      default:
        continue;
    }
  }

  back:;
  clear();
  *stage = 1;

}

static void reset() {
  if (win) delwin(win);
  clear();
  refresh();
  win = newwin(HEIGHT, WIDTH, center(LINES, HEIGHT), center(COLS, WIDTH));
  keypad(win, TRUE);
}

static void display(const int selected) {
  int i;
  box(win, 0, 0);
  // mvwaddwstr(win, 0, 0,                  L"╭");
  // mvwaddwstr(win, 0, WIDTH - 1,          L"╮");
  // mvwaddwstr(win, HEIGHT - 1, 0,         L"╰");
  // mvwaddwstr(win, HEIGHT - 1, WIDTH - 1, L"╯");
  wmove(win, 0, center(WIDTH, 12));
  waddch(win, ACS_RTEE);
  wprintw(win, " Settings ");
  waddch(win, ACS_LTEE);

  for (i = 0; i < n_entries; ++i) {
    if (selected == i) select(i);
    else unselect(i);
  }
  wrefresh(win);
}

static void select(const int selected) {
  int cur = selected + 2;
  int len = strlen(entries[selected]);
  wattron(win, A_REVERSE);
  mvwprintw(win, cur, 2, "%s%*s", entries[selected], 28 - len, "");
  wattroff(win, A_REVERSE);
}

static void unselect(const int selected) {
  int cur = selected + 2;
  mvwprintw(win, cur, 2, entries[selected]);
}