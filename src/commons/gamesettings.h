#ifndef H_GAME_SETTINGS_INCLUDED
#define H_GAME_SETTINGS_INCLUDED

#define newSettings(size, winCondition) (GameSettings){size, winCondition, 1}
#define DEFAULT_SETTINGS &newSettings(4, 2048)

typedef struct {
    int gridSize;
    int winCondition;
    int tileSpacing;
} GameSettings;

#endif