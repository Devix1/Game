#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
using namespace std;

const int WIDTH = 30;
const int HEIGHT = 20;

char field[HEIGHT][WIDTH];

struct Bullet {
    int x, y;
    char dir;
    bool active;
    bool fromEnemy;
};

struct Tank {
    int x, y;
    char dir;
    bool alive;
    int respawnCounter = 0;
};

Tank player;
vector<Tank> enemies;
vector<Bullet> bullets;

int playerLives = 3;
int score = 0;
int enemyRespawnTime = 30;

void setCursorPosition(int x, int y) {
    setlocale(LC_ALL, "RU");
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { SHORT(x), SHORT(y) };
    SetConsoleCursorPosition(hOut, coord);
}

void initField() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1)
                field[y][x] = '#';
            else
                field[y][x] = ' ';
        }

    for (int x = 6; x < 24; x++) field[6][x] = '#';
    for (int x = 3; x < 10; x++) field[12][x] = '+';
    for (int y = 3; y < 10; y++) field[y][15] = '+';
}

void spawnEnemies() {
    enemies.push_back({ 2, 2, 'D', true });
    enemies.push_back({ WIDTH - 3, 2, 'D', true });
}

void drawField() {
    setCursorPosition(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            bool drawn = false;

            
            if (player.alive) {
                if (player.dir == 'U') {
                    if (x == player.x && y == player.y - 1) { cout << '^'; drawn = true; }
                    else if (x == player.x && y == player.y) { cout << 'O'; drawn = true; }
                }
                else if (player.dir == 'D') {
                    if (x == player.x && y == player.y) { cout << 'O'; drawn = true; }
                    else if (x == player.x && y == player.y + 1) { cout << 'v'; drawn = true; }
                }
                else if (player.dir == 'L') {
                    if (x == player.x - 1 && y == player.y) { cout << '<'; drawn = true; }
                    else if (x == player.x && y == player.y) { cout << 'O'; drawn = true; }
                }
                else if (player.dir == 'R') {
                    if (x == player.x && y == player.y) { cout << 'O'; drawn = true; }
                    else if (x == player.x + 1 && y == player.y) { cout << '>'; drawn = true; }
                }
            }

            
            if (!drawn) {
                for (Tank& e : enemies) {
                    if (e.alive && x == e.x && y == e.y) {
                        switch (e.dir) {
                        case 'U': cout << '^'; break;
                        case 'D': cout << 'v'; break;
                        case 'L': cout << '<'; break;
                        case 'R': cout << '>'; break;
                        default: cout << 'E'; break;
                        }
                        drawn = true;
                        break;
                    }
                }
            }

            
            if (!drawn) {
                bool bulletDrawn = false;
                for (Bullet& b : bullets) {
                    if (b.active && b.x == x && b.y == y) {
                        cout << '*';
                        bulletDrawn = true;
                        break;
                    }
                }

                if (!bulletDrawn)
                    cout << field[y][x];
            }
        }
        cout << endl;
    }
    cout << "Жизни: " << playerLives << " Очки: " << score << " " << endl;
}


void shoot(Tank& shooter, bool fromEnemy = false) {
    Bullet b;
    b.x = shooter.x;
    b.y = shooter.y;
    b.dir = shooter.dir;
    b.active = true;
    b.fromEnemy = fromEnemy;

    switch (b.dir) {
    case 'U': b.y--; break;
    case 'D': b.y++; break;
    case 'L': b.x--; break;
    case 'R': b.x++; break;
    }

    if (b.x >= 0 && b.x < WIDTH && b.y >= 0 && b.y < HEIGHT)
        bullets.push_back(b);
}

void moveBullets() {
    for (Bullet& b : bullets) {
        if (!b.active) continue;

        switch (b.dir) {
        case 'U': b.y--; break;
        case 'D': b.y++; break;
        case 'L': b.x--; break;
        case 'R': b.x++; break;
        }

        if (b.x < 0 || b.x >= WIDTH || b.y < 0 || b.y >= HEIGHT) {
            b.active = false;
            continue;
        }

        char& cell = field[b.y][b.x];
        if (cell == '#') {
            b.active = false;
        }
        else if (cell == '+') {
            cell = ' ';
            b.active = false;
        }
        else {
            if (b.fromEnemy && player.alive && b.x == player.x && b.y == player.y) {
                playerLives--;
                b.active = false;
                if (playerLives <= 0) player.alive = false;
            }

            for (Tank& e : enemies) {
                if (e.alive && b.x == e.x && b.y == e.y && !b.fromEnemy) {
                    e.alive = false;
                    e.respawnCounter = enemyRespawnTime;
                    b.active = false;
                    score += 100;
                    break;
                }
            }
        }
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(),
        [](Bullet& b) { return !b.active; }),
        bullets.end());
}

void handleInput() {
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
        case 'w': player.dir = 'U'; if (field[player.y - 1][player.x] == ' ') player.y--; break;
        case 's': player.dir = 'D'; if (field[player.y + 1][player.x] == ' ') player.y++; break;
        case 'a': player.dir = 'L'; if (field[player.y][player.x - 1] == ' ') player.x--; break;
        case 'd': player.dir = 'R'; if (field[player.y][player.x + 1] == ' ') player.x++; break;
        case ' ': shoot(player); break;
        }
    }
}

bool hasLineOfSight(Tank& enemy) {
    if (enemy.x == player.x) {
        int step = (enemy.y < player.y) ? 1 : -1;
        for (int y = enemy.y + step; y != player.y; y += step)
            if (field[y][enemy.x] != ' ') return false;
        enemy.dir = (enemy.y < player.y) ? 'D' : 'U';
        return true;
    }
    else if (enemy.y == player.y) {
        int step = (enemy.x < player.x) ? 1 : -1;
        for (int x = enemy.x + step; x != player.x; x += step)
            if (field[enemy.y][x] != ' ') return false;
        enemy.dir = (enemy.x < player.x) ? 'R' : 'L';
        return true;
    }
    return false;
}

void moveEnemies() {
    for (Tank& e : enemies) {
        if (!e.alive) {
            if (--e.respawnCounter <= 0) {
                e.x = rand() % (WIDTH - 4) + 2;
                e.y = rand() % 5 + 1;
                e.dir = 'D';
                e.alive = true;
            }
            continue;
        }

        if (hasLineOfSight(e)) {
            if (rand() % 4 == 0) shoot(e, true);
            continue;
        }

        if (rand() % 2 == 0) {
            if (e.y < player.y && field[e.y + 1][e.x] == ' ') { e.y++; e.dir = 'D'; }
            else if (e.y > player.y && field[e.y - 1][e.x] == ' ') { e.y--; e.dir = 'U'; }
        }
        else {
            if (e.x < player.x && field[e.y][e.x + 1] == ' ') { e.x++; e.dir = 'R'; }
            else if (e.x > player.x && field[e.y][e.x - 1] == ' ') { e.x--; e.dir = 'L'; }
        }
    }
}
 

int main() {
    srand(time(0));
    player = { WIDTH / 2, HEIGHT - 2, 'U', true };
    initField();
    spawnEnemies();

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    while (true) {
        if (!player.alive) {
            setCursorPosition(0, HEIGHT + 2);
            cout << "Игра окончена! Ваш счёт: " << score << "   " << endl;
            break;
        }

        handleInput();
        moveBullets();
        moveEnemies();
        drawField();
        Sleep(100);
    }

    system("pause");
    return 0;
}
