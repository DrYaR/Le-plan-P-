#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
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
    int knowledge;
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

void drawGameOverScreen() {
    FILE *file = fopen("gameover.txt", "r");
    if (file == NULL) {
        perror("Unable to open gameover file");
        exit(EXIT_FAILURE);
    }

    char line[131];
    while (fgets(line, sizeof(line), file) != NULL) {
        for (int i = 0; i < strlen(line); i++) {
            if (line[i] == '1') {
                printf("\033[37m❌\033[0m");
            } else if (line[i] == '0') {
                printf("  ");
            } else {
                printf("%c", line[i]);
            }
        }
        printf("\n");
    }
    fclose(file);
}
void drawMap(char **map, int width, int height, Player *player) {
    int displayWidth = 124;
    int displayHeight = 46;

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
            } else if (map[y][x] == 'p') {
                printf(">");
            } else if (map[y][x] == '0') {
                printf(" ");
            } else if (map[y][x] == 'f') {
                printf("H");
            } else if (map[y][x] == 'u') {
                printf(" ");
            } else if (map[y][x] == 'a') {
                printf("\033[40m \033[0m");
            } else if (map[y][x] == 'z') {
                printf("\033[47m \033[0m");
            } else if (map[y][x] == 'b') {
                printf("\033[40m \033[0m");
            } else if (map[y][x] == 'V') {
                printf("\033[30m👺\033[0m");
            } else if (map[y][x] == 'c') {
                printf(" ");
            } else if (map[y][x] == 'o') {
                printf("H");
            } else if (map[y][x] == 'y') {
                printf("|");
            } else if (map[y][x] == 'd') {
                printf(" ");
            } else if (map[y][x] == 'x') {
                printf("\033[31m2\033[0m");
            } else if (map[y][x] == '9') {
                printf("\033[31mV\033[0m");
            } else if (map[y][x] == 'i') {
                printf("\033[31m0\033[0m");                
            } else {
                printf("%c", map[y][x]);
            }
        }
        if (y == startY) printf("   Энергия: %s%.1f%s", (player->energy < 10 ? "\033[31m" : ""), player->energy, (player->energy < 10 ? "\033[0m" : ""));
        if (y == startY + 1) printf("   Сытость: %s%.1f%s", (player->hunger < 10 ? "\033[31m" : ""), player->hunger, (player->hunger < 10 ? "\033[0m" : ""));
        if (y == startY + 5) printf("   Знания: %s%d%s", (player->knowledge < 10 ? "\033[31m" : ""), player->knowledge, (player->knowledge < 10 ? "\033[0m" : ""));
        if (y == startY + 2) printf("   Менталочка: %s%.1f%s", (player->mental < 10 ? "\033[31m" : ""), player->mental, (player->mental < 10 ? "\033[0m" : ""));
        if (y == startY + 3) printf("   День %s%d%s", (player->Days > 25 ? "\033[31m" : ""), player->Days, (player->Days > 25 ? "\033[0m" : ""));
        if (y == startY + 4) printf("   Время: %s%d%s", (player->hour > 23 ? "\033[31m" : ""), player->hour, (player->hour > 23 ? "\033[0m" : ""));
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

    printf("\nВыбери еду из холодильника:\n");
    for (int i = 0; i < sizeof(foodNames) / sizeof(foodNames[0]); i++) {
        printf("%d. %s (+%d Сытость)\n", i + 1, foodNames[i], foodValues[i]);
    }

    char choiceStr[2];
    scanf("%1s", choiceStr);
    int choice = atoi(choiceStr);
    if (choice > 0 && choice <= sizeof(foodNames) / sizeof(foodNames[0])) {
        player->hunger += foodValues[choice - 1];
        if (player->hunger > MAX_STAT) player->hunger = MAX_STAT;
    }
}

///КОМП///
int pcMenu(Player *player) {
    if (player->energy < 27) {
        printf("Ты не можешь сейчас работать, ты устал..");
        return 0;}  
    
    printf("\nChoose task for now:\n");
    printf("1. Сегодняшний проект (+2 Знания, -6 Часов, -25 Энергия, от -1 до -6 Менталочка)\n");
    printf("2. Послушать лекцию (+1 Знания, -10 Энергия, -2 Менталочка)\n");

    char choice = getchar();
    if (choice == '1') {
        player->energy -= 25;
        player->knowledge += 2;
        player->mental -= 1 + rand() % 5;
        if (player->hour <= 17) {player->hour += 6;}
        else {player->Days += 1; player->hour = 0;}
        if (player->Days == 28) {
            drawGameOverScreen();
            exit(EXIT_SUCCESS);
        }
    } else if (choice == '2') {
        player->energy -= 10;
        player->knowledge += 1;
        player->mental -= 2;
        player->min += 45;
    }
return 0;
}

///ЛЕКЦИИ слушать///
int lectureMenu(Player *player) {
    if (player->energy < 12) {
        printf("Ты не можешь сейчас слушать лекцию, ты устал..");
        return 0;}   
    
    printf("\nChoose task for now:\n");
    printf("1. Слушать лекцию (+1 Знания,-30 минут, -10 Энергия)\n");


    char choice = getchar();
    if (choice == '1') {
        player->energy -= 10;
        player->knowledge += 1;
        player->min += 30;
        if (player->min >= 60) {
            player->min -= 60;
            player->hour += 1;
        }
        if (player->hour >= 24) {
            player->hour -= 24;
            player->Days += 1;
            if (player->Days == 28) {
                drawGameOverScreen();
                exit(EXIT_SUCCESS);
            }
        }
    }
return 0;
}

///ЛЕКЦИИ рассказывать///
int speakMenu(Player *player) {
    if (player->energy < 22) {
        printf("Ты не можешь сейчас провести лекцию, ты устал..");
        return 0;}   
    
    printf("\nChoose task for now:\n");
    printf("1. Провести лекцию (-30 минут, -20 Энергия)\n");


    char choice = getchar();
    if (choice == '1') {
        player->energy -= 20;
        player->min += 30;
        if (player->min >= 60) {
            player->min -= 60;
            player->hour += 1;
        }
        if (player->hour >= 24) {
            player->hour -= 24;
            player->Days += 1;
            if (player->Days == 28) {
                drawGameOverScreen();
                exit(EXIT_SUCCESS);
            }
        }
    }
return 0;
}

///ПИРЫ///
int peerMenu(Player *player) {
    if (player->energy < 12) {
        printf("Ты не можешь сейчас разговаривать с пиром, ты устал..");
        return 0;}   
    
    printf("\nChoose task for now:\n");
    printf("1. Поговорить (-5 минут, -10 Энергия, +1 или -1 Менталочка, +1 или +0 Знания)\n");


    char choice = getchar();
    if (choice == '1') {
        player->energy -= 10;
        player->min += 5;
        if (player->min >= 60) {
            player->min -= 60;
            player->hour += 1;
        }
        if (player->hour >= 24) {
            player->hour -= 24;
            player->Days += 1;
            if (player->Days == 28) {
                drawGameOverScreen();
                exit(EXIT_SUCCESS);
            }
        }
        if (rand() % 2 == 0) {
            player->mental -= 1;
        } else {
            player->mental += 1;
        }
        player->knowledge += rand() % 2;
    }
return 0;
}

///Раковина///
void washMenu(Player *player) { 
    
    printf("\nВыбери действие:\n");
    printf("1. Помыть руки (-2 минуты, +1 Менталочка)\n");


    char choice = getchar();
    if (choice == '1') {
        player->min += 2;
        if (player->mental >= MAX_STAT){
            player->mental = MAX_STAT;
        } else {player->mental += 1;}
    }
}
void movePlayer(Player *player, char direction, int width, int height, char **map) {
    int newX = player->x;
    int newY = player->y;
if (player->mental >= 30){
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

    if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] != '1' && map[newY][newX] != '2' && map[newY][newX] != 'V' && map[newY][newX] != '3' && map[newY][newX] != '4' && map[newY][newX] != '5' && map[newY][newX] != '7' && map[newY][newX] != '6' && map[newY][newX] != '#' && map[newY][newX] != '8' && map[newY][newX] != 'b' && map[newY][newX] != 'y' ) {
        player->x = newX;
        player->y = newY;
    }
}

long getTimeInMilliseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void movePlusSymbols(char **map, int width, int height, Player *player) {
     srand(time(NULL));
     if (player->mental >=30) {
     for (int y = 0; y < height; y++) {
         for (int x = 0; x < width; x++) {
             if (map[y][x] == '+') {
                int newX = x, newY = y;
                int dx = rand() % 3 - 1;
                int dy = rand() % 3 - 1;
                if (dx == 0 && dy == 0) {
                    // "застывание"
                } else if (dx == -1 && x == 0) {
                    newX = 0;
                } else if (dx == 1 && x == width - 1) {
                    newX = width - 1;
                } else if (dy == -1 && y == 0) {
                    newY = 0;
                } else if (dy == 1 && y == height - 1) {
                    newY = height - 1;
                } else {
                    newX += dx;
                    newY += dy;
                 }
                 // преграды
                 if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] == ' ' ) {
                     map[y][x] = ' ';
                     map[newY][newX] = '+';
                }
            }
        }
    }
    } else {for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == '+') {
                int newX = x;
                int newY = y;
                switch (rand() % 4) {
                    case 0:
                        newX++;
                        break;
                    case 1:
                        newY++;
                        break;
                    case 2:
                        newX--;
                        break;
                    case 3:
                        newY--;
                        break;
                }
                if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] != '*' && map[newY][newX] != '+' && map[newY][newX] != 'V') {
                    map[y][x] = ' ';
                    map[newY][newX] = '+';
                }
            }
        }
    }
    } 
}

void moveV(char **map, int width, int height, Player *player) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 'V') {
                int newX = x;
                int newY = y;
                int dx = player->x - newX;
                int dy = player->y - newY;
                if (dx != 0) {
                    newX += dx / abs(dx);
                }
                if (dy != 0) {
                    newY += dy / abs(dy);
                }
                if (newX == player->x && newY == player->y) {
                    player->energy -= 10.0;
                    player->hunger -= 10.0;
                    player->mental -= 10.0;
                }
                if (newX >= 0 && newX < width && newY >= 0 && newY < height && map[newY][newX] == ' ' || map[newY][newX] == '#'|| map[newY][newX] == '9' || map[newY][newX] == 'L' || map[newY][newX] == 'I' || map[newY][newX] == 'F' || map[newY][newX] == 'T'|| map[newY][newX] == 'V') {
                    map[y][x] = ' ';
                    map[newY][newX] = 'V';
                }
            }
        }
    }
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
    system("resize -s 64 190");
    Player player = {1, 1, 100.0, 100.0, 95.0, 1, 9, 00, 0};

    snprintf(filename, sizeof(filename), "map%d.txt", currentMap + 1);
    loadMapFromFile(map, width, height, filename, &player);
    movePlusSymbols(map, width, height, &player);

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
                        drawGameOverScreen();
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
                    player.y -=5;
                    player.hour = 9;
                    player.min = 0;

                };

                const char* fridgeType[] = {
                    "item"
                };
                const char* pc_type[] = {
                    "pc_event"
                // "lection"
                };
                const char* talk_type[] = {
                    "conversation"
                };
                const char* lection_type[] = {
                    "lection"
                };
                const char* bio_type[] = {
                    "bio"
                };
                 const char* speach_type[] = {
                    "speach"
                };
                /*
                 const char* набор_типов_действий[] = { //теперь, категорий может быть много
                    //"ещё_категория_действия",
                    "категория_действия"
                };
                */
                movePlayer(&player, command, width, height, map);

                if ((map[player.y][player.x] == 'x' || map[player.y][player.x] == 'i') && player.knowledge == 100) {
                    currentMap = (currentMap + 1) % maxMaps;
                    snprintf(filename, sizeof(filename), "Boss.txt", currentMap + 1);
                    loadMapFromFile(map, width, height, filename, &player);
                } else if (map[player.y][player.x] == 'B') {
                    displayFridgeMenu(&player);
                } else if (map[player.y][player.x] == 'o') {
                    pcMenu(&player);
                } else if (map[player.y][player.x] == 'f') {
                    lectureMenu(&player);
                } else if (map[player.y][player.x] == 'u') {
                    speakMenu(&player);
                } else if (map[player.y][player.x] == 'p') {
                    washMenu(&player); 
                } else if (map[player.y][player.x] == '+') {
                    peerMenu(&player);    
            }
        }  
        }

        long currentTime = getTimeInMilliseconds();
        if (currentTime - lastUpdateTime >= 1000) {
            movePlusSymbols(map, width, height, &player);
            moveV(map, width, height, &player);
            if ((player.energy <= 0.0) || (player.hunger <= 0.0)){
                drawGameOverScreen();
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

