#define _XOPEN_SOURCE_EXTENDED

#include <curses.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils/common.h"
#include "stages/settings.h"

#define WIDTH 32
#define HEIGHT 8

static void reset();

static void display(const int _selected);

static void _select(const int _selected);

static void _deselect(const int _selected);

static const char *entries[] = {
    "Board size",
    "Win condition",
    "Tile spacing",
    "Back",
};

static const int n_entries = sizeof(entries) / sizeof(char *);

static WINDOW *win = NULL;
static int _selected = 0;

void stageSettings(int *stage, GameSettings *settings) {
  int l;
  for (;;) {
    reset();
    display(_selected);
    l = numlen(settings->gridSize);
    mvwprintw(win, 2, 28 - l - 2, "[ %d ]", settings->gridSize);
    l = numlen(settings->winCondition);
    mvwprintw(win, 3, 28 - l - 2, "[ %d ]", settings->winCondition);
    l = settings->tileSpacing ? numlen(settings->tileSpacing) : 1;
    mvwprintw(win, 4, 28 - l - 2, "[ %d ]", settings->tileSpacing);
    wrefresh(win);
    switch (tolower(wgetch(win))) {
      case 0x1b:
      case 'q':
        _selected = 3;
        goto back;
      case 'w':
      case KEY_UP:
        if (--_selected < 0) _selected = n_entries - 1;
        break;
      case 's':
      case KEY_DOWN:
        if (++_selected == n_entries) _selected = 0;
        break;
      case '\n':
      case KEY_ENTER:
      case KEY_RIGHT:
        switch (_selected) {
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
        switch (_selected) {
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
  wclear(win);
  wrefresh(win);
  delwin(win);
  win = NULL;
  *stage = 1;

}

static void reset() {
  if (!win) {
    win = newwin(HEIGHT, WIDTH, center(LINES, HEIGHT), center(COLS, WIDTH));
    keypad(win, TRUE);
  }
  wrefresh(win);
}

static void display(const int _selected) {
  int i;
  box(win, 0, 0);
  mvwaddwstr(win, 0, 0,                  L"╭");
  mvwaddwstr(win, 0, WIDTH - 1,          L"╮");
  mvwaddwstr(win, HEIGHT - 1, 0,         L"╰");
  mvwaddwstr(win, HEIGHT - 1, WIDTH - 1, L"╯");
  wmove(win, 0, center(WIDTH, 12));
  waddch(win, ACS_RTEE);
  wprintw(win, " Settings ");
  waddch(win, ACS_LTEE);

  for (i = 0; i < n_entries; ++i) {
    if (_selected == i) _select(i);
    else _deselect(i);
  }
  wrefresh(win);
}

void _select(const int _selected) {
  int cur = _selected + 2;
  int len = strlen(entries[_selected]);
  wattron(win, A_REVERSE);
  mvwprintw(win, cur, 2, "%s%*s", entries[_selected], 28 - len, "");
  wattroff(win, A_REVERSE);
}

static void _deselect(const int _selected) {
  int cur = _selected + 2;
  int len = strlen(entries[_selected]);
  mvwprintw(win, cur, 2, "%s%*s", entries[_selected], 28 - len, "");
}
