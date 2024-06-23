#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "map.h"
#include "player.h"


// Функция для конфигурации терминала, отключающая буферизацию ввода
void disableBuffering() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Функция для восстановления конфигурации терминала по умолчанию
void enableBuffering() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Функция для рисования карты
void drawMap(char **map, int width, int height, Player *player) {
    system("clear"); // Очистка экрана
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == player->x && y == player->y) {
                // Зеленый цвет для игрока
                printf("\033[32m*\033[0m"); 
            } else if (map[y][x] == 'D') {
                // Красный цвет для двери
                printf("\033[31mD\033[0m");
            } else if (map[y][x] == '#') {
                // Коричневый цвет для стен
                printf("\033[33m#\033[0m");
            } else {
                // Обычные символы
                printf("%c", map[y][x]);
            }
        }
        printf("\n");
    }
}

// Главная функция
int main() {
    int width = 100;
    int height = 36;
    int maxMaps = 3;
    int currentMap = 0; // Начинаем с первой карты
    char filename[20];

    // Выделяем память для карты
    char **map = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        map[i] = (char *)malloc(width * sizeof(char));
    }

    // Инициализируем игрока
    Player player = {1, 1}; // Начальная позиция игрока

    // Загружаем карту
    snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
    loadMapFromFile(map, width, height, filename);

    // Отключаем буферизацию ввода
    disableBuffering();

    // Главный цикл игры
    char command;
    while (1) {
        // Отрисуем карту
        drawMap(map, width, height, &player);

        // Считываем команду пользователя
        command = getchar();

        // Обрабатываем команду
        if (command == 'q') break;

        // Перемещаем игрока
        movePlayer(&player, command, width, height, map);

        // Проверяем, наступил ли игрок на дверь
        if (map[player.y][player.x] == 'D') {
            currentMap = (currentMap + 1) % maxMaps; // Переход на следующую карту (циклически)
            snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
            loadMapFromFile(map, width, height, filename);
            player.x = 1; // Начальная позиция на новой карте
            player.y = 1;
        }
    }

    // Восстанавливаем буферизацию ввода
    enableBuffering();

    // Освобождаем память для карты
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);

    return 0;
}