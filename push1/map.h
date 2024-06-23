#ifndef MAP_H
#define MAP_H
#include "player.h"
void loadMapFromFile(char **map, int width, int height, const char *filename);
void drawMap(char **map, int width, int height, Player *player);

#endif