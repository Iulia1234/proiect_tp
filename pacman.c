#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <stdio.h>

#define WIDTH 40 
#define HEIGHT 20 
#define PACMAN 'S' 
#define WALL '#' 
#define FOOD '.' 
#define EMPTY ' ' 
#define DEMON 'N'

int res = 0; //rezultat joc(0=continui, 1=pierdut, 2=castigat)
int score = 0; //scor
int pacman_x, pacman_y; //pozitia
char board[HEIGHT][WIDTH]; //harta 2D
int food = 0; //mancarea totala generata
int curr = 0; //mancarea adunata

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* textures[7];

int load_textures() {
    const char* files[7] = {
        "grass.png",
        "stone.png",
        "ball.png",
        "ninja1.png",
        "samurai1.png",
        "samura2.png",
        "ninja2.png"
    };

    for (int i = 0; i < 7; i++) {
        SDL_Surface* surface = IMG_Load(files[i]);
        if (!surface) {
            printf("Failed to load %s: %s\n", files[i], IMG_GetError());
            return 0;
        }
        textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!textures[i]) {
            printf("Failed to create texture from %s: %s\n", files[i], SDL_GetError());
            return 0;
        }
    }
    return 1;
}

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

        SDL_RenderClear(renderer);
        SDL_Texture* samurai=(frame%2==0) ? textures[3]: textures[5];
        SDL_Texture* ninja=(frame%2==0) ? textures[4]: textures[6];

        for(int i=0; i<HEIGHT; i++)
        {
            for(int j=0; j<WIDTH; j++)
            {
                SDL_Rect dest={j*32, i*32, 32, 32};
                switch(board[i][j])
                {
                    case WALL:
                    SDL_RenderCopy(renderer, textures[1], NULL, &dest); break;

                    case EMPTY:
                    SDL_RenderCopy(renderer, textures[0], NULL, &dest); break;

                    case FOOD:
                    SDL_RenderCopy(renderer, textures[2], NULL, &dest); break;

                    case PACMAN:
                    SDL_RenderCopy(renderer, samurai, NULL, &dest); break;

                    case DEMON:
                    SDL_RenderCopy(renderer, ninja, NULL, &dest); break;
                }
            }
        }

        SDL_RenderPresent(renderer);
        frame++;
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

void process_input(){
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type==SDL_QUIT)
        {
            res=1;

        }
        else if(event.type==SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_w: move_pacman(0,-1); break;
                case SDLK_s: move_pacman(0,1); break; 
                case SDLK_a: move_pacman(-1,0); break;
                case SDLK_d: move_pacman(1,0); break;
                case SDLK_q: res=1; break;
            }
        }
    }
}
int frame=0;
int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)!=0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    window=SDL_CreateWindow("Pac-Man SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*32, HEIGHT*32, SDL_WINDOW_SHOWN);
    if(!window){
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    renderer=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    if(!load_textures())
    {
        prinft("Failed to load textures.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    initialize();
    int running=1;
    while(running && !res){
        process_inout();
        draw();
        SDL_Delay(100);
    }
    SDL_Delay(500);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    SDL_Surface* msgSurface;
    SDL_Texture* msgTexture;
    SDL_Color white={255, 255, 255, 255};

    if(res==1)
    {
        printf("Game Over! Eaten by a ninja.\n");
        printf("Final Score: %d\n", score);

    }
    else if(res==2)
    {
        printf("You Win!\n");
        printf("Final Score: %d\n", score);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(3000);

    for(int i=0; i<5; i++)
    {
        if(textures[i])
        {
            SDL_DestroyTexture(textures[i]);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
