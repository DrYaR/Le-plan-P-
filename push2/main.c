#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

typedef struct {
    int x;
    int y;
    float energy;
    float hunger;
    float mental;
} Player;

void disableBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON;
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enableBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON;
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void loadMapFromFile(char **map, int width, int height, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Не удалось открыть файл карты");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = fgetc(file);
            if (c == '\n' || c == EOF) break;
            map[y][x] = c;
        }
        if (fgetc(file) == EOF) break;  // Читаем символ новой строки
    }

    fclose(file);
}

void drawMap(char **map, int width, int height, Player *player) {
    system("clear"); // Очистка экрана
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == player->x && y == player->y) {
                printf("\033[32m*\033[0m");
            } else if (map[y][x] == 'D') {
                printf("\033[31mD\033[0m");
            } else if (map[y][x] == '#') {
                printf("\033[33m#\033[0m");
            } else {
                printf("%c", map[y][x]);
            }
        }
        if (y == 0) printf("   Энергия: %s%.1f%s", (player->energy < 10 ? "\033[31m" : ""), player->energy, (player->energy < 10 ? "\033[0m" : ""));
        if (y == 1) printf("   Сытость: %s%.1f%s", (player->hunger < 10 ? "\033[31m" : ""), player->hunger, (player->hunger < 10 ? "\033[0m" : ""));
        if (y == 2) printf("   Психологическое состояние: %s%.1f%s", (player->mental < 10 ? "\033[31m" : ""), player->mental, (player->mental < 10 ? "\033[0m" : ""));
        printf("\n");
    }
}

void movePlayer(Player *player, char direction, int width, int height, char **map) {
    int newX = player->x;
    int newY = player->y;

    if (direction == 'w') newY--;
    if (direction == 's') newY++;
    if (direction == 'a') newX--;
    if (direction == 'd') newX++;

    if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] != '#') {
        player->x = newX;
        player->y = newY;
    }
}

long getTimeInMilliseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
    int width = 100;
    int height = 36;
    int maxMaps = 3;
    int currentMap = 0;
    char filename[20];

    char **map = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        map[i] = (char *)malloc(width * sizeof(char));
    }

    Player player = {1, 1, 100.0, 100.0, 100.0};

    snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
    loadMapFromFile(map, width, height, filename);

    disableBuffering();

    char command;
    long lastUpdateTime = getTimeInMilliseconds();
    while (1) {
        drawMap(map, width, height, &player);

        fd_set set;
        struct timeval timeout;

        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; 

        if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) == 1) {
            if (read(STDIN_FILENO, &command, 1) == 1) {
                if (command == 'q') break;movePlayer(&player, command, width, height, map);

                if (map[player.y][player.x] == 'D') {
                    currentMap = (currentMap + 1) % maxMaps;
                    snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
                    loadMapFromFile(map, width, height, filename);
                    player.x = 1;
                    player.y = 1;
                }
            }
        }

        long currentTime = getTimeInMilliseconds();
        if (currentTime - lastUpdateTime >= 1000) {
            player.energy -= 0.1;
            player.hunger -= 0.1;
            player.mental -= 0.1;
            lastUpdateTime = currentTime;
        }
    }

    enableBuffering();

    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);

    return 0;
}