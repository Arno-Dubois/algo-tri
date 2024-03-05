#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <math.h>

#define FPS 60
#define WIDTH 1442
#define HEIGHT 600
#define SIZE 200

typedef struct Box {
    int value;
    struct Box *next;
    struct Box *previous;
    struct BoxList *boxList;
} Box;

typedef struct BoxList {
    struct Box *head;
    struct Box *tail;
} BoxList;

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    if (TTF_Init() != 0) {
        printf("Error initializing SDL TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    SDL_Window *wind = SDL_CreateWindow("Algo tri dans ma boite",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WIDTH, HEIGHT, 0);
    if (!wind) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, render_flags);
    if (!rend) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }

    bool running = true;
    SDL_Event event;


    while (running) {




    SDL_SetRenderDrawColor(rend, 16, 16, 16, 255);
    SDL_RenderClear(rend);











    SDL_RenderPresent(rend);
    SDL_Delay(1000/FPS);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);    
    TTF_Quit();
    SDL_Quit();
    return 0;
}