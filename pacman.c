#include <ncurses.h> 
#include <stdlib.h> 

#define WIDTH 40 
#define HEIGHT 20 
#define PACMAN 'C' 
#define WALL '#' 
#define FOOD '.' 
#define EMPTY ' ' 
#define DEMON 'X'

int res = 0; //rezultat joc(0=continui, 1=pierdut, 2=castigat)
int score = 0; //scor
int pacman_x, pacman_y; //pozitia
char board[HEIGHT][WIDTH]; //harta 2D
int food = 0; //mancarea totala generata
int curr = 0; //mancarea adunata

//initializarea hartii
void initialize() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || j == WIDTH - 1 || j == 0 || i == HEIGHT - 1) {
                board[i][j] = WALL; //marginile
            } else {
                board[i][j] = EMPTY; //interiorul gol
            }
        }
    }
    //am adaugat 50 de pereti aleatorii
    int count = 50;
    while (count) {
        int i = rand() % HEIGHT;
        int j = rand() % WIDTH;
        if (board[i][j] == EMPTY) {
            board[i][j] = WALL;
            count--;
        }
    }

    int val = 5;
    while (val--) {
        int row = rand() % HEIGHT;
        for (int j = 3; j < WIDTH - 3; j++) {
            if (board[row][j] == EMPTY) {
                board[row][j] = WALL;
            }
        }
    }
    //am pus 10 demoni aleatori
    count = 10;
    while (count) {
        int i = rand() % HEIGHT;
        int j = rand() % WIDTH;
        if (board[i][j] == EMPTY) {
            board[i][j] = DEMON;
            count--;
        }
    }
    //pacman in mijloc
    pacman_x = WIDTH / 2;
    pacman_y = HEIGHT / 2;
    board[pacman_y][pacman_x] = PACMAN;

    //am pus mancarea pe pozitiile pare
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i % 2 == 0 && j % 2 == 0 &&
                board[i][j] == EMPTY) {
                board[i][j] = FOOD;
                food++;
            }
        }
    }
}
//desenez harta
void draw() {
    clear();  //sterge tot
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvaddch(i, j, board[i][j]);  //afiseaza fiecare caracter
        }
    }

    mvprintw(HEIGHT, 0, "Score: %d", score);  //scor sub harta
}
//functie de mutare pacman
void move_pacman(int move_x, int move_y) {
    int x = pacman_x + move_x;
    int y = pacman_y + move_y;

    if (board[y][x] != WALL) {
        if (board[y][x] == FOOD) {
            score++;
            food--;
            curr++;
            if (food == 0) {
                res = 2; //win
                return;
            }
        } else if (board[y][x] == DEMON) {
            res = 1; //lost
        }

        board[pacman_y][pacman_x] = EMPTY;
        pacman_x = x;
        pacman_y = y;
        board[pacman_y][pacman_x] = PACMAN;
    }
}

int main() {
    //aici am folosit functii din ncurses
    initscr(); //initializeaza fereastra ncurses
    noecho(); //nu afisa tastele
    cbreak(); //input fara enter
    curs_set(0); //ascunde cursorul
    keypad(stdscr, TRUE); //activeaza tastele sageti
    nodelay(stdscr, TRUE); 

    initialize(); //jocul

    mvprintw(HEIGHT + 3, 0, "Use W A S D to move. Press Q to quit.");
    mvprintw(HEIGHT + 4, 0, "Press Y to start...");
    refresh();

    int ch;
    while ((ch = getch()) != 'y' && ch != 'Y') {
        napms(100); //pauza
    }

    while (1) {
        draw();

        if (res == 1) {
            clear();
            mvprintw(HEIGHT / 2, (WIDTH - 20) / 2, "Game Over! Eaten by demon!");
            mvprintw(HEIGHT / 2 + 1, (WIDTH - 20) / 2, "Final Score: %d", score);
            refresh();
            napms(3000);
            break;
        }

        if (res == 2) {
            clear();
            mvprintw(HEIGHT / 2, (WIDTH - 10) / 2, "You Win!");
            mvprintw(HEIGHT / 2 + 1, (WIDTH - 20) / 2, "Final Score: %d", score);
            refresh();
            napms(3000);
            break;
        }

        ch = getch();
        switch (ch) {
            case 'w': move_pacman(0, -1); break;
            case 's': move_pacman(0, 1); break;
            case 'a': move_pacman(-1, 0); break;
            case 'd': move_pacman(1, 0); break;
            case 'q': endwin(); return 0;
        }

        napms(100); //intarziere intre pasi
    }

    endwin(); //opresc ncurses
    return 0;
}
