#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x;
    int y;
} Player;

void movePlayer(Player *player, char direction, int width, int height, char **map);

#endif