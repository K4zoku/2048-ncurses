#include <stdlib.h>

#include "2048/2048.h"
#include "utils/common.h"

static bool withinBounds(Board *this, int index) {
  return inRangeE(index, 0, this->size * this->size);
}

static bool withinBound(Board *this, int x, int y) {
  return inRangeE(x, 0, this->size) && inRangeE(y, 0, this->size);
}

Board *newBoard(int size) {
  Board *this = (Board *) malloc(sizeof(Board));
  this->size = size;
  this->tiles = (int *) calloc(size * size, sizeof(int));
  this->moves = 0;
  this->scores = 0;
  return this;
}

int getIndex(Board *this, int x, int y) {
  int result = x + y * this->size;
  if (!withinBound(this, x, y)) return -1;
  return withinBounds(this, result) ? result : -1;
}

int getIndexByDirection(Board *this, int x, int y, Direction direction) {
  int size, fullSize, result;
  if (!withinBound(this, x, y)) return -1;
  size = this->size;
  fullSize = size * size - 1;
  switch (direction) {
    case LEFT:
      result = x + y * size;
      break;
    case RIGHT:
      result = fullSize - (x + y * size);
      break;
    case UP:
      result = y + x * size;
      break;
    case DOWN:
      result = fullSize - (y + x * size);
      break;
    default:
      // HOW THE F***?
      result = -1;
      break;
  }
  return withinBounds(this, result) ? result : -1;
}


bool cellAvailable(Board *this, int index) {
  return this->tiles[index] == 0;
}

bool cellsAvailable(Board *this) {
  int i, len;
  len = this->size * this->size;
  for (i = 0; i < len; ++i) {
    if (cellAvailable(this, i)) {
      return true;
    }
  }
  return false;
}

int *availableCells(Board *this, size_t *len) {
  uint i;
  int *result;
  size_t size;
  size = this->size * this->size;
  result = malloc(size * sizeof(int));
  *len = 0;
  for (i = 0; i < size; ++i) {
    if (cellAvailable(this, i)) {
      result[(*len)++] = i;
    }
  }
  if (*len < size) {
    result = (int *) realloc(result, (*len) * sizeof(int));
  }
  return result;
}

int randomAvailableCell(Board *this) {
  size_t len;
  int *cells = availableCells(this, &len);
  return len > 0 ? cells[rand() % len] : -1;
}

void addRandomTile(Board *this) {
  int cell = randomAvailableCell(this);
  if (cell == -1) return;
  this->tiles[cell] = ((rand() % 100) < 90) ? 2 : 4;
}

void addRandomTiles(Board *this, int n_tiles) {
  while (n_tiles--) addRandomTile(this);
}

bool moveTiles(Board *this, Direction direction) {
  bool merged = false, moved = false;
  int i, x, y, index, next, previous;
  for (y = 0; y < this->size; ++y) {
    previous = getIndexByDirection(this, 0, y, direction);
    for (x = 1; x < this->size; ++x) {
      index = getIndexByDirection(this, x, y, direction);
      if (this->tiles[index]) {
        if (this->tiles[index] == this->tiles[previous]) {
          merged = true;
          this->tiles[previous] <<= 1;
          this->tiles[index] = 0;
          this->scores += this->tiles[previous];
        }
        previous = index;
      }
    }

    for (x = 0; x < this->size; ++x) {
      index = getIndexByDirection(this, x, y, direction);
      if (this->tiles[index] == 0) {
        for (i = x + 1; i < this->size; ++i) {
          next = getIndexByDirection(this, i, y, direction);
          if (this->tiles[next] != 0) {
            moved = true;
            this->tiles[index] = this->tiles[next];
            this->tiles[next] = 0;
            break;
          }
        }
      }
    }
  }

  if (moved || merged) {
    ++this->moves;
    return true;
  }

  return false;
}