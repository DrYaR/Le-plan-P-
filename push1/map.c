#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "player.h"

// Функция для загрузки карты из файла
void loadMapFromFile(char **map, int width, int height, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open map file");
        exit(EXIT_FAILURE);
    }

    // Считываем карту из файла
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int ch = fgetc(file);
            if (ch == EOF) break; // Конец файла
            if (ch != '\n') {
                map[y][x] = ch;
            } else {
                x--; // Уменьшаем x, чтобы не пропустить символ
            }
        }
        fgetc(file); // Читаем символ новой строки
    }

    fclose(file);
}
