#ifndef H_2048_INCLUDED
#define H_2048_INCLUDED

#include <stdbool.h>

typedef enum {
    LEFT=0, UP=1, RIGHT=2, DOWN=3
} Direction;

typedef struct Board {
    int size;
    int *tiles;
    int moves;
    long scores;
} Board;

Board *newBoard(int size);

bool moveTiles(Board *this, Direction direction);

int getIndex(Board *this, int x, int y);

int getIndexByDirection(Board *this, int x, int y, Direction direction);

bool cellAvailable(Board *this, int index);

bool cellsAvailable(Board *this);

int *availableCells(Board *this, size_t *len);

int randomAvailableCell(Board *this);

void addRandomTile(Board *this);

void addRandomTiles(Board *this, int n_tiles);

#endif
