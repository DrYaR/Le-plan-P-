#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define MAX_STAT 100.0
#define MIN_STAT 12.0

typedef struct {
    int x;
    int y;
    float energy;
    float hunger;
    float mental;
    int Days;
    int hour;
    int min;
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

void loadMapFromFile(char **map, int width, int height, const char *filename, Player *player) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open map file");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = fgetc(file);
            if (c == '\n' || c == EOF) break;
            map[y][x] = c;
            if (c == 'd') {
                player->x = x;
                player->y = y;
            }
        }
        if (fgetc(file) == EOF) break;
    }

    fclose(file);
}

void drawMap(char **map, int width, int height, Player *player) {
    int displayWidth = 100;
    int displayHeight = 25;

    int startX = player->x - displayWidth / 2;
    int startY = player->y - displayHeight / 2;

    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (startX + displayWidth > width) startX = width - displayWidth;
    if (startY + displayHeight > height) startY = height - displayHeight;

    printf("\033[H\033[J");
    for (int y = startY; y < startY + displayHeight; y++) {
        for (int x = startX; x < startX + displayWidth; x++) {
            if (x == player->x && y == player->y) {
                printf("\033[32m*\033[0m");
            } else if (map[y][x] == 'D') {
                printf("\033[31mD\033[0m");
            } else if (map[y][x] == '1') {
                printf("\033[31m#\033[0m");
            } else if (map[y][x] == '8') {
                printf("\033[36m#\033[0m");
            } else if (map[y][x] == '#') {
                printf("\033[30m#\033[0m");
            } else if (map[y][x] == '2') {
                printf("\033[30m|\033[0m");
            } else if (map[y][x] == '3') {
                printf("\033[30m-\033[0m");
            } else if (map[y][x] == '7') {
                printf("\033[30m]\033[0m");
            } else if (map[y][x] == '6') {
                printf("\033[30m[\033[0m");
            } else if (map[y][x] == 'B') {
                printf("\033[34mB\033[0m");
            } else if (map[y][x] == '5') {
                printf("\033[35m#\033[0m");
            } else if (map[y][x] == '4') {
                printf("_");
            } else if (map[y][x] == '0') {
                printf(" ");
            } else if (map[y][x] == 'f') {
                printf("H");
            } else if (map[y][x] == 'a') {
                printf("\033[40m \033[0m");
            } else if (map[y][x] == 'b') {
                printf("\033[40m \033[0m");
            } else if (map[y][x] == 'c') {
                printf(" ");
            } else if (map[y][x] == 'o') {
                printf("H");
            } else if (map[y][x] == 'd') {
                printf(" ");              
            } else {
                printf("%c", map[y][x]);
            }
        }
        if (y == startY) printf("   Energy: %s%.1f%s", (player->energy < 10 ? "\033[31m" : ""), player->energy, (player->energy < 10 ? "\033[0m" : ""));
        if (y == startY + 1) printf("   Hunger: %s%.1f%s", (player->hunger < 10 ? "\033[31m" : ""), player->hunger, (player->hunger < 10 ? "\033[0m" : ""));
        if (y == startY + 2) printf("   Mental: %s%.1f%s", (player->mental < 10 ? "\033[31m" : ""), player->mental, (player->mental < 10 ? "\033[0m" : ""));
        if (y == startY + 3) printf("   Day %s%d%s", (player->Days > 25 ? "\033[31m" : ""), player->Days, (player->Days > 25 ? "\033[0m" : ""));
        if (y == startY + 4) printf("   Time: %s%d%s", (player->hour > 23 ? "\033[31m" : ""), player->hour, (player->hour > 23 ? "\033[0m" : ""));
        if (y == startY + 4) {
            char buffer[3];
            sprintf(buffer, "%02d", player->min);
            printf(":%s%s%s", (player->min > 90 ? "\033[31m" : ""), buffer, (player->min > 90 ? "\033[0m" : ""));
        }
        printf("\n");
    }
}

// холодильник
void displayFridgeMenu(Player *player) {
    const char *foodNames[] = {"Яблоко", "Пицца", "Суп", "Салат"};
    const int foodValues[] = {2, 3, 1, 1};
    const char *foodActions[] = {"Яблоко", "Пицца", "Суп", "Салат"};
    const int foodCount = sizeof(foodNames) / sizeof(foodNames[0]);

    printf("\nВыбери еду из холодильника:\n");
    for (int i = 0; i < foodCount; i++) {
        printf("%d. %s (+%d hunger)\n", i + 1, foodNames[i], foodValues[i]);
    }
    printf("\nВведите номер пункта для его активации: ");
    char choiceStr[2];
    scanf("%1s", choiceStr);
    int choice = atoi(choiceStr);
    if (choice > 0 && choice <= foodCount) {
        printf("Вы выбрали: %s\n", foodActions[choice - 1]);
        player->hunger += foodValues[choice - 1];
    }

    if (player->hunger > MAX_STAT) player->hunger = MAX_STAT;
    if (player->hunger < 0) player->hunger = 0;
}

///КОМП///
int pcMenu(Player *player) {
    if (player->energy < MIN_STAT) {
        printf("Ты не можешь сейчас работать, ты устал..");
        return 0;}
    if (player->mental < MIN_STAT) {
        printf("Ты не можешь сейчас работать, ты устал..");
        return 0;}    
    
    printf("\nChoose task for now:\n");
    printf("1. Сегодняшний проект (-15 energy, -3 mental)\n");
    printf("2. Послушать лекцию (-5 energy, -1 mantal)\n");




    char choice = getchar();
    if (choice == '1') {
        player->energy -= 10;
        player->mental -= 1 + rand() % 5;
    } else if (choice == '2') {
        player->energy -= 4;
        player->mental -= 2;
    }

}

///ЛЕКЦИИ///
int lectureMenu(Player *player) {
    if (player->energy < MIN_STAT) {
        printf("Ты не можешь сейчас слушать лекцию, ты устал..");
        return 0;}   
    
    printf("\nChoose task for now:\n");
    printf("1. Слушать лекцию (-15 energy)\n");


    char choice = getchar();
    if (choice == '1') {
        player->energy -= 15;
    }

}

void movePlayer(Player *player, char direction, int width, int height, char **map) {
    int newX = player->x;
    int newY = player->y;
if (player->mental >= 15){
    if (direction == 'w') newY--;
    if (direction == 's') newY++;
    if (direction == 'a') newX--;
    if (direction == 'd') newX++;
} else {
    if (direction == 'w') newY++;
    if (direction == 's') newY--;
    if (direction == 'a') newX++;
    if (direction == 'd') newX--;
}

    if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] != '1' && map[newY][newX] != '2' && map[newY][newX] != '3' && map[newY][newX] != '4' && map[newY][newX] != '5' && map[newY][newX] != '7' && map[newY][newX] != '6' && map[newY][newX] != '#' && map[newY][newX] != '8' && map[newY][newX] != 'b' ) {
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
    int width = 190;
    int height = 64;
    int maxMaps = 3;
    int currentMap = 0;
    char filename[20];
    char **map = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        map[i] = (char *)malloc(width * sizeof(char));
    }

    Player player = {1, 1, 100.0, 100.0, 95.0, 1, 9, 00};

    snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
    loadMapFromFile(map, width, height, filename, &player);


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
                if (command == 'q') break;
                if (map[player.y][player.x] == 'c') {
                    if (player.Days >= 28) {
                        printf("GAME OVER");
                        return 0;
                    }
                    player.energy = 100.0;
                    player.hunger = 100.0;
                    if (player.mental >= MAX_STAT) {player.mental = MAX_STAT;}
                    else if (player.mental < 0) {player.mental = 0;}
                    else {player.mental += 5.0;}
                    if (player.hour >= 1){
                        player.Days += 1;
                    }
                    player.y -=4;
                    player.hour = 9;
                    player.min = 0;

                };

                movePlayer(&player, command, width, height, map);

                if (map[player.y][player.x] == 'D') {
                    currentMap = (currentMap + 1) % maxMaps;
                    snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
                    loadMapFromFile(map, width, height, filename, &player);
                } else if (map[player.y][player.x] == 'B') {
                    displayFridgeMenu(&player);
                } else if (map[player.y][player.x] == 'o') {
                    pcMenu(&player);
                } else if (map[player.y][player.x] == 'f') {
                    lectureMenu(&player);
                }
            }
        }

        long currentTime = getTimeInMilliseconds();
        if (currentTime - lastUpdateTime >= 1000) {
            if ((player.energy <= 0.0) || (player.hunger <= 0.0)){
                printf("GAME OVER\n");
                break;}
            player.energy -= 0.02;
            player.hunger -= 0.1;
            if (player.min < 59 ) {player.min += 1;}
            else {player.hour += 1;
            player.min = 0;}
            if (player.hour == 23 && player.min == 59) {
                player.Days += 1;
                player.hour = 0;
                player.min = 0;
            }
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