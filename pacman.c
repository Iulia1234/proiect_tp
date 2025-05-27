#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
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

int res = 0; // 0=continui, 1=pierdut, 2=castigat
int score = 0; 
int pacman_x, pacman_y; 
char board[HEIGHT][WIDTH]; 
int food = 0; 
int curr = 0; 

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* textures[7];
Mix_Chunk* sound_eat = NULL;
Mix_Chunk* sound_lose = NULL;
Mix_Chunk* sound_win = NULL;
Mix_Music* bg_music = NULL;
TTF_Font* font = NULL;

int frame = 0;

#include <stdio.h>

int load_textures() {
    const char* files[7] = {
        "/home/debian/iulia/grass.png",
        "/home/debian/iulia/stone.png",
        "/home/debian/iulia/ball.png",
        "/home/debian/iulia/ninja1.png",
        "/home/debian/iulia/samurai1.png",
        "/home/debian/iulia/samurai2.png",
        "/home/debian/iulia/ninja2.png"
    };
    for (int i = 0; i < 7; i++) {
        printf("Loading texture: %s\n", files[i]);
        
        // Verifică dacă fișierul există înainte de încărcare
        FILE* f = fopen(files[i], "rb");
        if (!f) {
            printf("ERROR: File %s not found or cannot be opened.\n", files[i]);
            return 0;
        }
        fclose(f);

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


int load_sounds() {
    sound_eat = Mix_LoadWAV("/home/debian/iulia/eat.wav");
    sound_lose = Mix_LoadWAV("/home/debian/iulia/hurt.wav");
    sound_win = Mix_LoadWAV("/home/debian/iulia/win.wav");
    bg_music = Mix_LoadMUS("/home/debian/iulia/bgmusic.ogg");
    if (!sound_eat || !sound_lose || !sound_win || !bg_music) {
        printf("Failed to load sound: %s\n", Mix_GetError());
        return 0;
    }
    return 1;
}

void initialize() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || j == WIDTH - 1 || j == 0 || i == HEIGHT - 1) {
                board[i][j] = WALL;
            } else {
                board[i][j] = EMPTY;
            }
        }
    }
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
    count = 3;
    while (count) {
        int i = rand() % HEIGHT;
        int j = rand() % WIDTH;
        if (board[i][j] == EMPTY) {
            board[i][j] = DEMON;
            count--;
        }
    }
    pacman_x = WIDTH / 2;
    pacman_y = HEIGHT / 2;
    board[pacman_y][pacman_x] = PACMAN;

    food = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i % 2 == 0 && j % 2 == 0 && board[i][j] == EMPTY) {
                board[i][j] = FOOD;
                food++;
            }
        }
    }
}

void draw_score() {
    SDL_Color white = {255, 255, 255, 255};
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d", score);

    SDL_Surface* surface = TTF_RenderText_Solid(font, score_text, white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    SDL_Rect dst = {10, 10, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

void draw() {
    SDL_RenderClear(renderer);
    SDL_Texture* samurai = textures[3];
    SDL_Texture* ninja = textures[4];


    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            SDL_Rect dest = {j * 32, i * 32, 32, 32};
            switch (board[i][j]) {
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
    draw_score();
    SDL_RenderPresent(renderer);
    frame++;
}

void move_pacman(int move_x, int move_y) {
    int x = pacman_x + move_x;
    int y = pacman_y + move_y;

    if (board[y][x] != WALL) {
        if (board[y][x] == FOOD) {
            score++;
            food--;
            curr++;
            Mix_PlayChannel(-1, sound_eat, 0);
            if (food == 0) {
                res = 2;
                return;
            }
        } else if (board[y][x] == DEMON) {
            res = 1;
            Mix_PlayChannel(-1, sound_lose, 0);
        }
        board[pacman_y][pacman_x] = EMPTY;
        pacman_x = x;
        pacman_y = y;
        board[pacman_y][pacman_x] = PACMAN;
    }
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            res = 1;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w: move_pacman(0, -1); break;
                case SDLK_s: move_pacman(0, 1); break;
                case SDLK_a: move_pacman(-1, 0); break;
                case SDLK_d: move_pacman(1, 0); break;
                case SDLK_q: res = 1; break;
            }
        }
    }
}

void render_text_centered(const char* text, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {(WIDTH * 32 - w) / 2, y, w, h};

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

void show_start_screen() {
    // Încarcă imaginea de fundal
    SDL_Surface* surface = IMG_Load("/home/debian/iulia/startscreen.png");
    if (!surface) {
        printf("Failed to load background image: %s\n", IMG_GetError());
    } else {
        SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!background) {
            printf("Failed to create background texture: %s\n", SDL_GetError());
        } else {
            SDL_RenderCopy(renderer, background, NULL, NULL);
            SDL_DestroyTexture(background);
        }
    }

    // Pornește muzica de fundal
    if (bg_music) {
        Mix_PlayMusic(bg_music, -1);
    }

    // Poziționează textul de start (subtil)
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, "Press ENTER to start", textColor);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    int text_w = 0, text_h = 0;
    SDL_QueryTexture(text_texture, NULL, NULL, &text_w, &text_h);

    SDL_Rect text_rect = {
        (WIDTH - text_w) / 2,
        HEIGHT - text_h - 20, // puțin deasupra marginii de jos
        text_w,
        text_h
    };

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                Mix_HaltMusic();
                return;
            }
            if (e.type == SDL_QUIT) exit(0);
        }
        SDL_Delay(100);
    }
}


// O nouă funcție care desenează text cu „shadow”
void render_text_centered_shadow(const char* text, int y, SDL_Color color, SDL_Color shadow_color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, shadow_color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {(WIDTH * 32 - w) / 2 + 2, y + 2, w, h}; // Umbra la (2,2)

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);

    // Acum desenează textul real deasupra umbrei
    surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    dst.x -= 2;
    dst.y -= 2;
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}


void show_end_screen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const char* msg = (res == 2) ? "You Win! Congratulations!" : "Game Over! Eaten by a ninja.";
    render_text_centered(msg, HEIGHT * 7);

    char score_str[64];
    snprintf(score_str, sizeof(score_str), "Final Score: %d", score);
    render_text_centered(score_str, HEIGHT * 10);

    render_text_centered("Press ESC to exit", HEIGHT * 13);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return;
            if (e.type == SDL_QUIT) exit(0);
        }
        SDL_Delay(100);
    }
}

void move_demons_towards_pacman() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] == DEMON) {
                int dx = pacman_x - j;
                int dy = pacman_y - i;
                int step_x = 0, step_y = 0;

                // Aleg direcția în funcție de care diferență este mai mare
                if (abs(dx) > abs(dy)) {
                    step_x = (dx > 0) ? 1 : -1;
                } else if (dy != 0) {
                    step_y = (dy > 0) ? 1 : -1;
                }

                int new_x = j + step_x;
                int new_y = i + step_y;

                // Verific dacă pot să mă mut pe noua poziție
                if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
                    if (board[new_y][new_x] == PACMAN) {
                        // Demonul a ajuns la Pacman => pierzi
                        res = 1;
                        Mix_PlayChannel(-1, sound_lose, 0);
                        return;
                    }
                    else if (board[new_y][new_x] == EMPTY || board[new_y][new_x] == FOOD) {
                        board[i][j] = EMPTY;
                        board[new_y][new_x] = DEMON;
                    } else {
                        // Dacă nu pot merge în direcția dorită, încerc pe cealaltă axă
                        if (step_x != 0 && dy != 0) {
                            new_x = j;
                            step_y = (dy > 0) ? 1 : -1;
                            new_y = i + step_y;
                            if (new_y >= 0 && new_y < HEIGHT &&
                                (board[new_y][new_x] == EMPTY || board[new_y][new_x] == FOOD)) {
                                board[i][j] = EMPTY;
                                board[new_y][new_x] = DEMON;
                            }
                        }
                        else if (step_y != 0 && dx != 0) {
                            new_y = i;
                            step_x = (dx > 0) ? 1 : -1;
                            new_x = j + step_x;
                            if (new_x >= 0 && new_x < WIDTH &&
                                (board[new_y][new_x] == EMPTY || board[new_y][new_x] == FOOD)) {
                                board[i][j] = EMPTY;
                                board[new_y][new_x] = DEMON;
                            }
                        }
                    }
                }
            }
        }
    }
}
 int demon_move_counter=0;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio Error: %s\n", Mix_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Samurai vs Ninja", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * 32, HEIGHT * 32, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }

    font = TTF_OpenFont("OpenSans-Regular.ttf", 28);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    if (!load_textures() || !load_sounds()) {
        printf("Failed to load assets\n");
        return 1;
    }

    Mix_PlayMusic(bg_music, -1);

    show_start_screen();

    initialize();

    while (res == 0) {
    process_input();

    demon_move_counter++;
    if (demon_move_counter >= 5) {  // demonii se mișcă o dată la 5 frame-uri (5 * 100ms = 500ms)
        move_demons_towards_pacman();
        demon_move_counter = 0;
    }

    draw();
    SDL_Delay(100);
}



    Mix_HaltMusic();

    if (res == 2) Mix_PlayChannel(-1, sound_win, 0);
    else if (res == 1) Mix_PlayChannel(-1, sound_lose, 0);

    show_end_screen();

    // Cleanup
    for (int i = 0; i < 7; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    Mix_FreeChunk(sound_eat);
    Mix_FreeChunk(sound_lose);
    Mix_FreeChunk(sound_win);
    Mix_FreeMusic(bg_music);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
return 0;
}
