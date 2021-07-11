#define _XOPEN_SOURCE_EXTENDED

#include <ncursesw/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdbool.h>
#include <time.h>

#include "utils/common.h"
#include "stages/gameplay.h"
#include "2048/2048.h"

#define RATIO 2
#define TILE_HEIGHT 5
#define TILE_WIDTH TILE_HEIGHT * RATIO

void display();

void printTile(int y, int x, int value);

char *elapsedTime(llu startTime);

bool tileMatchesAvailable();

bool moveAvailable();

bool isWin(const int winCondition);

Board *board;
WINDOW *win;
int width, height;

int tile_margin = 1;
int window_padding = 0;

void stageGameplay(int *stage, GameSettings *settings) {
  unsigned long long startTime = time(0);
  chtype ch;
  WINDOW *scoreboard, *sub;
  int size = settings->gridSize;
  int starty, startx;
  bool keep = false;
  Direction direction;
  tile_margin = settings->tileSpacing;
  *stage = 1;
  height = 2 * (window_padding + 1) + size * TILE_HEIGHT + (size + 1) * tile_margin;
  width = 2 * (window_padding + 1) * RATIO + size * TILE_WIDTH + (size + 1) * tile_margin * RATIO;
  starty = center(LINES, height);
  startx = center(COLS, width);
  clear();
  win = newwin(height, width, starty, startx);
  scoreboard = newwin(5, 27, starty, startx + width + 2);
  box(scoreboard, 0, 0);
  keypad(win, TRUE);
  nodelay(win, TRUE);
  wtimeout(win, 500);
  wbkgd(win, COLOR_PAIR(8));
  box(win, 0, 0);
  mvwprintw(scoreboard, 1, 1, "Score: %16llu", 0);
  wrefresh(scoreboard);

  board = newBoard(size);
  addRandomTiles(board, 2);

  for (;;) {
    display();
    mvwprintw(scoreboard, 1, 1, "Score: %16d", board->scores);
    mvwprintw(scoreboard, 2, 1, "Moves: %16d", board->moves);
    wrefresh(scoreboard);
    ch = wgetch(win);
    mvwprintw(scoreboard, 3, 1, "Time elapsed: %9s", elapsedTime(startTime));
    wrefresh(scoreboard);
    switch (ch) {
      case 0x1b:
        goto end;
      case 'w':
      case 'W':
      case KEY_UP:
        direction = UP;
        break;
      case 'd':
      case 'D':
      case KEY_RIGHT:
        direction = RIGHT;
        break;
      case 's':
      case 'S':
      case KEY_DOWN:
        direction = DOWN;
        break;
      case 'a':
      case 'A':
      case KEY_LEFT:
        direction = LEFT;
        break;
      case ERR:
        // NO Key Pressed
        continue;
      default:
        continue;
    }
    if (!keep && isWin(settings->winCondition)) {
      sub = newwin(5, 31, center(LINES, 4), center(COLS, 27));
      box(sub, 0, 0);
      mvwprintw(sub, 0, center(31, 9), " YOU WIN ");
      mvwprintw(sub, 1, 2, "Press SPACE to continue");
      mvwprintw(sub, 2, 2, "Press ENTER to restart");
      mvwprintw(sub, 3, 2, "Press ESC to return to menu");
      wrefresh(sub);
      wrefresh(win);
      wtimeout(win, -1);
      wwait:;
      switch (wgetch(win)) {
        case ' ':
          keep = true;
          delwin(sub);
          wtimeout(win, 500);
          wbkgd(win, COLOR_PAIR(8));
          box(win, 0, 0);
          break;
        case '\n':
          *stage = 2;
        case 0x1b:
          goto end;
        default:
          goto wwait;
      }
    } else if (moveAvailable()) {
      if (moveTiles(board, direction)) {
        addRandomTile(board);
      }
    } else {
      sub = newwin(4, 31, center(LINES, 4), center(COLS, 27));
      box(sub, 0, 0);
      mvwprintw(sub, 0, center(31, 10), " YOU LOSE ");
      mvwprintw(sub, 1, 2, "Press ENTER to restart");
      mvwprintw(sub, 2, 2, "Press ESC to return to menu");
      wrefresh(sub);
      wrefresh(win);
      wtimeout(win, -1);
      lwait:;
      switch (wgetch(win)) {
        case '\n':
          delwin(sub);
          *stage = 2;
        case 0x1b:
          goto end;
        default:
          goto lwait;
      }
      break;
    }
  }
  end:;
  delwin(win);
}

void display() {
  int x, y;
  int size = board->size;
  int tile;
  for (x = 0; x < size; ++x)
    for (y = 0; y < size; ++y) {
      tile = getIndex(board, x, y);
      printTile(y, x, board->tiles[tile]);
    }
}

const uint colors[] = {
    COLOR_PAIR(8),
    COLOR_PAIR(1),
    COLOR_PAIR(2),
    COLOR_PAIR(3),
    COLOR_PAIR(4),
    COLOR_PAIR(5),
    COLOR_PAIR(6),
};

uint getColor(int value) {
  int i = 0;
  while (value) {
    ++i;
    value >>= 1;
  }
  return i ? colors[i % 6 + 1] : colors[0];
}

void printTile(int y, int x, int value) {
  uint color, i;
  size_t padlen, len;
  // calculate real coordinate
  y = y * TILE_HEIGHT + (window_padding + 1) + (y + 1) * tile_margin;
  x = x * TILE_WIDTH + RATIO * (window_padding + 1) + (x + 1) * tile_margin * RATIO;
  color = getColor(value);
  wattron(win, color);
  // fill
  for (i = 0; i < TILE_HEIGHT; ++i) {
    mvwprintw(win, y + i, x, "%*s", TILE_WIDTH, "");
  }
  if (value) {
    len = numlen(value);
    padlen = pad(len, TILE_WIDTH - 2);
    wmove(win, y + center(TILE_HEIGHT, 1), x + center(TILE_WIDTH, 8));
    wprintw(win, "%*s%d%*s", padlen, "", value, padlen, "");
  }
  wattroff(win, color);
  wrefresh(win);
}

char *elapsedTime(llu startTime) {
  char *result = (char *) calloc(16, sizeof(char));
  uint hh, mm, ss, elapsed, now = time(0);
  elapsed = now - startTime;
  hh = elapsed / 3600;
  elapsed %= 3600;
  mm = elapsed / 60;
  elapsed %= 60;
  ss = elapsed;
  if (hh) sprintf(result, "%3d:%02d:%02d", hh, mm, ss);
  else if (mm) sprintf(result, "%6d:%02d", mm, ss);
  else sprintf(result, "%8ds", ss);
  return result;
}

bool tileMatchesAvailable() {
  const int size = board->size;
  int y, x;
  int tile, other;
  Direction direction;
  for (direction = UP; direction <= RIGHT; ++direction) {
    for (x = 0; x < size; ++x) {
      for (y = 0; y < size; ++y) {
        tile = getIndexByDirection(board, x, y, direction);
        if (board->tiles[tile]) {
          other = getIndexByDirection(board, x + 1, y, direction);
          if (other != -1 && board->tiles[tile] == board->tiles[other]) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool moveAvailable() {
  return cellsAvailable(board) || tileMatchesAvailable();
}

bool isWin(const int winCondition) {
  int x, y;
  const int size = board->size;
  for (x = 0; x < size; ++x)
    for (y = 0; y < size; ++y) {
      if (board->tiles[getIndex(board, x, y)] == winCondition) return true;
    }
  return false;
}