#define _XOPEN_SOURCE_EXTENDED

#include <curses.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils/common.h"
#include "stages/menu.h"

#define WIDTH 22
#define HEIGHT 8

static void reset();

static void display(const int selected);

static void select(const int selected);

static void unselect(const int selected);

static const char *entries[] = {
    "Play",
    "Help",
    "Settings",
    "Quit",
};

static const int n_entries = sizeof(entries) / sizeof(char *);

static WINDOW *win = NULL;
static int selected = 0;

void stageMenu(int *stage, __attribute__((unused)) GameSettings *settings) {
  for (;;) {
    reset();
    display(selected);
    next:;
    switch (tolower(wgetch(win))) {
      case 0x1b:
      case 'q':
        selected = 3;
        goto changeStage;
      case 'w':
      case 'k':
      case KEY_UP:
        if (--selected < 0) selected = n_entries - 1;
        break;
      case 's':
      case 'j':
      case KEY_DOWN:
        if (++selected == n_entries) selected = 0;
        break;
      case '\n':
      case KEY_ENTER:
      case KEY_RIGHT:
      case KEY_LEFT:
      case 'h':
      case 'l':
        goto changeStage;
      default:
        goto next;
    }
  }

  changeStage:;
  wclear(win);
  wrefresh(win);
  delwin(win);
  win = NULL;
  switch (selected) {
    case 0:
      *stage = 2;
      break;
    case 1:
      *stage = 3;
      break;
    case 2:
      *stage = 4;
      break;
    case 3:
      *stage = 0;
      break;
  }
}

static void reset() {
  if (!win) {
    win = newwin(HEIGHT, WIDTH, center(LINES, HEIGHT), center(COLS, WIDTH));
    keypad(win, TRUE);
  }
  wrefresh(win);
}

static void display(const int selected) {
  int i;
  box(win, 0, 0);
  mvwaddwstr(win, 0, 0,                  L"╭");
  mvwaddwstr(win, 0, WIDTH - 1,          L"╮");
  mvwaddwstr(win, HEIGHT - 1, 0,         L"╰");
  mvwaddwstr(win, HEIGHT - 1, WIDTH - 1, L"╯");
  wmove(win, 0, center(WIDTH, 8));

  waddch(win, ACS_RTEE);
  waddch(win, ' ');
  wattron(win, A_REVERSE);
  waddch(win, '2' | COLOR_PAIR(1));
  waddch(win, '0' | COLOR_PAIR(2));
  waddch(win, '4' | COLOR_PAIR(3));
  waddch(win, '8' | COLOR_PAIR(4));
  wattroff(win, A_REVERSE);
  waddch(win, ' ');
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
  int padlen = (18 - len) / 2;
  wattron(win, A_REVERSE);
  mvwprintw(win, cur, center(WIDTH, 18), formatCenter(entries[selected], padlen));
  mvwaddch(win, cur, padlen, '>' | A_BLINK);
  mvwaddch(win, cur, padlen + len + 3, '<' | A_BLINK);
  wattroff(win, A_REVERSE);
}

static void unselect(const int selected) {
  int cur = selected + 2;
  int len = strlen(entries[selected]);
  int padlen = (18 - len) / 2;
  mvwprintw(win, cur, center(WIDTH, 18), formatCenter(entries[selected], padlen));
}
