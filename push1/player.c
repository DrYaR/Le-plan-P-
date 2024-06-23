#include "player.h"

void movePlayer(Player *player, char direction, int width, int height, char **map) {
    int newX = player->x;
    int newY = player->y;

    switch (direction) {
        case 'w':
            newY--;
            break;
        case 's':
            newY++;
            break;
        case 'a':
            newX--;
            break;
        case 'd':
            newX++;
            break;
    }

    // Проверка на границы карты и препятствия
    if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] != '#') {
        player->x = newX;
        player->y = newY;
    }
}